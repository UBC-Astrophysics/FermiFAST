import numpy as np
import sys

r1 = np.loadtxt(sys.argv[1],usecols=[1],unpack=True)
r1=np.sort(r1)

r2 = np.loadtxt(sys.argv[2],usecols=[1],unpack=True)
r2=np.sort(r2)

tot=len(r1)*len(r2)*1.0
if False:
    n2wins=0
    for a in r1:
        for b in r2:
            if (b>a):
                n2wins+=1

    n1wins=tot-n2wins

    print("list 1 wins= %g (%g)\nlist 2 wins= %g (%g)\n total = %g frac= %g\n" % (n1wins,n1wins/tot,n2wins,n2wins/tot,tot,2*n2wins/tot-1))

rall=np.concatenate((r1,r2))
n1all=np.concatenate((np.ones(len(r1)),np.zeros(len(r2))))
n2all=np.concatenate((np.zeros(len(r1)),np.ones(len(r2))))

index=np.argsort(rall)

rall=rall[index]
n1all=n1all[index]
n2all=n2all[index]

c1all=np.cumsum(n1all)
c2all=np.cumsum(n2all)

n1wins=np.sum(n1all*c2all)
n2wins=np.sum(n2all*c1all)

frac=2*n2wins/(len(r1)*len(r2))-1
#
# frac = 2 f2 - 1 = 2 (1-f1) -1 = 2 - 2 f1 -1 = 1 - 2 f1
#
print(sys.argv[1])
print("list 1 wins= %g (%g)\nlist 2 wins= %g (%g)\n total = %g (%g) frac= %g" % (n1wins,n1wins/tot,n2wins,n2wins/tot,tot,n2wins+n1wins-tot,frac))
print("%4d & %4d & %4.2f%%\n" % (frac*len(r1),(1-frac)*len(r1),frac*100)) 
