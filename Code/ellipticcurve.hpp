#include <iostream> 
#include <vector> 
#include <math.h> 
using namespace std; 

class EC
{
public:
    int a, b, x, y, p; 

public:

    EC()
    {
        this->a = 0;
        this->b = 0;
        this->x = 0;
        this->y = 0;
        this->p = 0;
    }

    EC(int a, int b, int x, int y, int p)
    {
        this->a = a;
        this->b = b;
        this->x = x;
        this->y = y;
        this->p = p;
    }

    EC neg(EC p)
    {
        if(p.x == 0 && p.y == 0)
            return p;
        
        return EC(p.a, p.b, p.x, (p.p-p.y) % p.p, p.p);
    }

    EC add(EC p2)
    {
        int x1 = this->x;
        int y1 = this->y;
        int x2 = p2.x;
        int y2 = p2.y;
        int p = this->p;
        int a = this->a;
        int b = this->b;

        int x3, y3, m; 

        if(x1 == 0 && y1 == 0) {
            return p2;
        }
        else if(x1 == x2 && (y1 == p-y2 || y1 == -y2)) {
            return EC(a, b, 0, 0, p); 
        }
        else if (x1 == x2 && y1 == y2)
        {
            m = ((((3 * x1 * x1) + a)%p) * modInverse(2 * y1, p)) % p;
        }
        else
        {
            m = (((y1 - y2)%p) * modInverse(x1 - x2, p)) % p;
        }

        x3 = ((m * m) - x1 - x2) % p;
        y3 = (m * (x3 - x1) + y1) % p;


        return EC(a, b, (x3+p) % p, (p-y3) % p, p);
    }

    EC sub(EC p2)
    {
        if(p2.x == 0 && p2.y == 0) {
            return *this;
        }

        EC temp = neg(p2); 
        return this->add(temp);
    }

    int modInverse(int a, int p)
    {
        if(a < 0) {
            return p - modInverse(-a, p);
        }

        int s = 0, old_s = 1; 
        int t = 1, old_t = 0;
        int r = p, old_r = a; 

        while(r != 0)
        {
            int quotient = old_r / r;
            int temp = r;
            r = old_r - quotient * r;
            old_r = temp;

            temp = s;
            s = old_s - quotient * s;
            old_s = temp;

            temp = t;
            t = old_t - quotient * t;
            old_t = temp;
        }

        return old_s % p; 
    }

    EC mul(int k)
    {
        EC res = EC(this->a, this->b, 0, 0, this->p);

        for (int i = 0; i < k; i++)
        {
            res = res.add(*this);
        }

        return res;
    } 

    void print()
    {
        cout << "(" << this->x << ", " << this->y << ")" << endl;
    }
};

