
syms b a
n = input('enter number of nodes:')

eq1=(-a + (2*(1-2*b)))/(3*(1-2*b)+(3*b*(1-(2*b)^5)))
eq2= -b + 1-((1-a)^(n-1))
[b a]=solve(eq1,eq2,b,a)
b=double(b(1)), a=double(a(1))

Ptr=1-((1-a)^(n))
Ps=(n*a*((1-a)^(n-1))) / Ptr

Tc=1+2                
Ts=1+1+1+1+10+1+1+2
S=(Ps*Ptr*1800)/((Ps*Ptr*Ts)+(Ptr*Tc*(1-Ps)))


