fn foo(mut a : i32, mut b : i32) -> i32
{
    let mut a;
    a=b;
    return a+foo(a,b);
}
fn foo2(mut c: i32)
{
    let mut x;
    if c > 1 {
        x=c;
    }
    while c>1{
        let mut c;
        c=5;
        if c>10{
        }
        else{
            c=c+1;
        }
    }
}
fn main(mut a :i32)
{
    let mut b : i32;
    a=1;
    b=666+999*(2<4);
    a=foo(0,0);
    foo2(foo(a,b));
}
