fn foo(mut a: i32,mut b:i32)->i32
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
        c=c*2;
    }
    while c>1{
        let mut c;
        c=5;
        if c>10{
            let mut d;
            //c=x;
        }
        else{
            c=c+1;
        }
    }
    return ;
    //return a+foo(a,b);
}
fn main(mut a :i32)
{
    let mut b : i32;
    a=1;

    b=666+999*(2<4);
    1;
    (((a)));
    //c;
    b=a*2;
    a=foo(0,0);
    foo(0,0);
    foo2(foo(a,b));
}
