#include <bits/stdc++.h>
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

int babyStepGiantStep(EC g, EC h, int N)
{
    int m = sqrt(N) + 1;

    EC *table = new EC[m];
    table[0] = EC(g.a, g.b, 0, 0, g.p);

    for (int i = 1; i < m; i++)
    {
        table[i] = table[i-1].add(g);
        //table[i].print();
    }

    EC *table2 = new EC[m];
    table2[0] = EC(h.a, h.b, h.x, h.y, h.p);
    EC mp = g.mul(m);

    for (int i = 1; i < m; i++)
    {
        table2[i] = table2[i - 1].sub(mp);
        // table2[i-1].print(); 
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (table[j].x == table2[i].x and table[j].y == table2[i].y)
            {
                return i * m + j;
            }
        }
    }

    return -1;
}

int pohligHellman(EC g, EC h, int N)
{
    /*
     * Exact prime factorization of N = q1^e1 * q2^e2 * ... * qk^ek
     */

    vector<int> primes;
    vector<int> exponents; 
    vector<int> factors;

    int n = N;

    for (int i = 2; i <= n; i++)
    {
        if (n % i == 0)
        {
            primes.push_back(i);
            factors.push_back(1);
            while (n % i == 0)
            {
                n /= i;
                factors[factors.size() - 1] *= i; 
            }
        }
    }

    for (int i = 0; i < primes.size(); i++)
    {
        exponents.push_back(log(factors[i]) / log(primes[i]));
    }

    /*
     * Compute T = {j(N/q P) | 0 <= j <= q-1}
     */
       
    vector<vector<EC>> T (primes.size());

    for (int i = 0; i < primes.size(); i++)
    {
        int q = primes[i];
        EC P = g.mul(N / q);

        for (int j = 0; j < q; j++)
        {
            T[i].push_back(P.mul(j));
        }
    }  

    /*
     * Compute the congruences x = a mod q^e
     */

    vector<vector<int>> congruences (primes.size(), vector<int> (1, 0));

    // // print congruences
    // for (int i = 0; i < primes.size(); i++)
    // {
    //     for (int j = 0; j < congruences[i].size(); j++)
    //     {
    //         cout << i << " " << congruences[i][j] << endl; 
    //     }
    // }

    for (int i = 0; i < primes.size(); i++)
    {
        vector<EC> QTable (exponents[i], h);

        // Finding the first k in the congruence 
        EC temp = h.mul(N/primes[i]);

        for (int j = 0; j < T[i].size(); j++)
        {
            if (T[i][j].x == temp.x and T[i][j].y == temp.y)
            {
                congruences[i].push_back(j);
                break;
            }
        }

        // Finding the successive QTable elements 

        // QTable[k] = QTable[k-1] - congruences[k-1] * q^k * g
        // search for QTable[k] in T[i] to find congruences[k]

        for (int j = 1; j < exponents[i]; j++)
        {
            QTable[j] = QTable[j-1].sub(g.mul(congruences[i][j] * pow(primes[i], j-1)));

            EC temp2 = QTable[j].mul(N/pow(primes[i], j+1));

            for (int k = 0; k < T[i].size(); k++)
            {
                if (temp2.x == T[i][k].x and temp2.y == T[i][k].y)
                {
                    congruences[i].push_back(k);
                    break;
                }
            }
        }
    }

    // solve the congruences obtained using chinese remainder theorem 
    vector<int> a (primes.size(), 0);

    for (int i = 0; i < primes.size(); i++)
    {
        for (int j = 1; j < congruences[i].size(); j++)
        {
            a[i] += congruences[i][j] * pow(primes[i], j-1);
        }
    }

    // solve the congruence x = a[i] mod factors[i] using chinese remainder theorem 
    int x = 0;
    int M = 1;

    for (int i = 0; i < primes.size(); i++)
    {
        M *= factors[i];
    }

    for (int i = 0; i < primes.size(); i++)
    {
        int Mi = M / factors[i];
        int Mi_inverse = 1;

        for (int j = 1; j < factors[i]; j++)
        {
            if ((Mi * j) % factors[i] == 1)
            {
                Mi_inverse = j;
                break;
            }
        }

        x += a[i] * Mi * Mi_inverse;
    }

    return x % M; 
}

int main()
{
    // test baby step giant step algorithm
    EC g = EC(0, 1, 60, 19, 599);
    EC h = EC(0, 1, 277, 239, 599);

    // Order of point P is bounded by Hasse's theorem
    pohligHellman(g, h, 600);   
    cout << "Discrete Logarithm of Q over P using Pohlig-Hellman is : " << pohligHellman(g, h, 600) << endl;

    cout << "Discrete Logarithm of Q over P using Shanks algorithm is : " << babyStepGiantStep(g, h, 599) << endl;

    return 0;
}