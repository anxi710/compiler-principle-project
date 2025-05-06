fn f1(mut a : i32, mut b : i32){
    a = 1;
    let mut c : i32 = 1 * 2 + 3 * 4 < 4 / 2 - 3 / 1;;
    return;
}
fn f2(mut x:[i32;3],mut y:(i32,i32,i32)) -> i32 {
    let a : i32 = 1;
    if a > 1 {
        return 2;
    } else if a > 0 {
        return 1;
    } else {
        return 0;
    }
    let mut n : i32;
    while n>0 {
        n = n-1;
    }
    for mut i in 1..n+1 {
        n=n-1;
        }
    loop {
        continue;
    }
    let mut b:i32=x[0];
    x[0] = 1;
    let mut c:i32=y.0;
    a.0=x[0]+x[0];
    a.1=a.0;
    return x[1]>a.1;
}
fn f3() -> i32 {
    let mut a : i32 = 1;
    let mut b : &mut i32 = &mut a;
    let mut c : i32 = *b;
    c = *b * 2;
    c = 2 * *b;
    c = *b + 2;
    c = 2 + *b * 2 + *b + 2;
    2 + *b;
    *b * 2 * 3 + 2 + 3 + *b;
    *b = 2;
    let x : i32 = 1;
    let y : & i32 = &x;
    let z : i32 = *y;
    return x+y+z;
}
fn f7_1(mut x: i32, mut y : i32) {
    let mut z = {
        let mut t = x * x + x;
        t = t + x * y;
        t + x * y;
        t
    };
}
fn f7_2(mut x : i32, mut y : i32) -> i32 {
    let mut t = x * x + x;
    t = t + x*y;
    t
}
fn f7_3(mut a : i32) {
    let mut b = if a > 0 {
        1
    } else{
        0
    };
}
fn f7_4() {
    let mut a = loop {
        break 2;
    };
}
fn f9(){
    let mut b: (&i32, &i32);
    let mut c: ((&i32, &i32), &i32);
    let mut d: &mut (i32, i32, i32);
    b = (2>5,a.0);
    c = (1,);
    d = ();
    let e: (i32,);
}
fn main(){
    ;;;
    let mut a : i32;
    ((((1))));
    (a);
    a;
    let mut a_array:[i32;3];
    let mut b:[&i32;3];
    let mut c:[[&i32;3];3];
    let mut d:&mut[[i32;3];3];
    a_array = [1,2,3];
    let a_tuple:(i32,i32,i32);
    a_tuple=(1,2,f3());
    f1(f2(a_array,a_tuple),666 + 999 / 1 > (2 * 2));
    return ;
}