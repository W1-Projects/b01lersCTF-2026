Kết quả reverse chắc chắn từ ROM

ROM có các chuỗi:

Press Button:
flag is 'bctf{<above chars>}'
your emulator sucks :(
1) Chuỗi “above chars” là chuỗi nút bấm

Trong ROM có bảng ký tự ở 0x08008ba4:

a b s S R L U D r l

Nó map trực tiếp tới thứ tự bit chuẩn của GBA:

a = A
b = B
s = Select
S = Start
R = Right
L = Left
U = Up
D = Down
r = R
l = L

Thứ tự bit này đúng với KEYINPUT của GBA: bit 0..9 lần lượt là A, B, Select, Start, Right, Left, Up, Down, R, L.

2) Input không được check theo “số lần bấm”, mà theo buffer 128 slot

Main ghi input vào buffer tại 0x030000a0, và đếm số slot ở 0x0300009c.

Mỗi lần bấm một nút one-hot ở bit k, chương trình append vào buffer mẫu:

k số 0
rồi một số 1 (0x8000)

Nghĩa là:

A → [1]
B → [0,1]
Select → [0,0,1]
Start → [0,0,0,1]
Right → [0,0,0,0,1]
…
L → [0,0,0,0,0,0,0,0,0,1]

Nên bài này thực chất là tìm một bitstream dài 128 slot, sau đó mới quy ngược thành chuỗi ký tự a b s S R L U D r l.

3) Khi đủ 128 slot, ROM gọi validator

Validator nằm ở 0x08000310. Nó không so sánh trực tiếp chuỗi bấm, mà gọi đi gọi lại một helper rất nhỏ đã copy vào IWRAM, entry ở 0x03000025.

Helper này:

bật IE với giá trị 4
gọi swi 2 (halt chờ interrupt)
động vào DMA0, DMA1, DMA3
đọc một word lệch hàng từ 0x0200000b
trả về low 16-bit làm output

Trọng điểm là validator đang biến 128 slot input thành một mạch logic 2-input nhiều tầng, chứ không phải check string kiểu thông thường.

Vì sao có chuỗi your emulator sucks :(

Init của ROM set DMA0/1/3 rồi gọi helper một lần để kiểm tra môi trường. Nếu helper trả về 0, game in ra your emulator sucks :( và treo.

Điều này rất hợp lý với tài liệu phần cứng GBA:

DMA0..DMA3 có ưu tiên theo số kênh; CPU bị pause khi DMA chạy.
Khi bật bit Enable từ 0 lên 1, phần cứng reload SAD/DAD/CNT_L vào internal registers.
HBlank DMA chỉ chạy khi tới HBlank; lần copy đầu tiên chỉ xảy ra sau khi line 0 đã được vẽ xong.
Trên ARM7/GBA, LDR lệch hàng đọc ở địa chỉ đã align rồi rotate theo (addr & 3)*8, nên một lệnh đọc từ 0x0200000b có thể biến thành “lấy byte/halfword rất dị” từ word căn thẳng hàng.
mGBA từng có cả bài dài về các bug DMA “holy grail” vì game thật sự có thể dựa vào hành vi DMA rất quái.

Nói ngắn gọn: chall này gần như chắc chắn cố tình lợi dụng timing/DMA behavior, nên emulator không chuẩn sẽ fail ngay từ check đầu.

Cách solve thực tế từ đây
Bước 1: chạy trên emulator chuẩn

Dùng mGBA hoặc no$gba. Tránh các emulator lỏng tay.

Đặt breakpoint/watchpoint vào:

0x08001634 — lúc main gọi validator
0x03000025 — helper IWRAM
0x0300009c — input count
0x030000a0 — input buffer
DMA regs:
0x040000ba = DMA0CNT_H
0x040000c6 = DMA1CNT_H
0x040000d4 = DMA3SAD
0x040000de = DMA3CNT_H
Bước 2: trích truth table của helper

Mỗi lần helper được gọi, input chỉ là:

r0 ∈ {0, 0x8000}
r1 ∈ {0, 0x8000}

Trong validator, DMA3SAD được đổi qua các bảng sau:

0x08008b86
0x08008b7c
0x08008b84
0x08008b8e
0x08008b7e
0x08008b94
0x08008b88

Vậy tổng cộng chỉ cần đo 7 × 4 = 28 trường hợp.

Làm xong bước này là bạn có một tập gate 2-input kiểu:

gate A: f(x,y)
gate B: g(x,y)
…

không cần hiểu chi tiết timing nữa.

Bước 3: rebuild validator thành boolean DAG

Validator là straight-line code rất dài. Pattern của nó là:

load 2 giá trị
gọi helper
lưu result xuống stack
load 2 result khác
gọi helper
…

Nên bạn có thể parse disassembly để dựng DAG:

lá: x0..x127 là 128 slot input
node trong: gate_type(left, right)
root: node cuối cùng trước đoạn
subs r3, r0, #1
sbcs r0, r3

Tức là validator chỉ cần output cuối khác 0.

Bước 4: solve bitstream 128 slot

Khi có công thức boolean cuối, ném vào SAT/Z3 là ra 128 bit.

Bước 5: đổi 128 slot về chuỗi nút

Dùng tokenization này:

tok = {
    'a': [1],
    'b': [0,1],
    's': [0,0,1],
    'S': [0,0,0,1],
    'R': [0,0,0,0,1],
    'L': [0,0,0,0,0,1],
    'U': [0,0,0,0,0,0,1],
    'D': [0,0,0,0,0,0,0,1],
    'r': [0,0,0,0,0,0,0,0,1],
    'l': [0,0,0,0,0,0,0,0,0,1],
}

Flag sẽ là:

bctf{<chuoi_ky_tu_o_tren_man_hinh>}