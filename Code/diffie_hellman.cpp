#include "ellipticcurve.hpp"
#include <assert.h>
#include <random>

// Baby Step Giant Step algorithm 
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

// diffie hellman key exchange encryption with random key generation
pair<vector<EC> , int>  ecdh_keygen(EC G)
{
    /*Alice generates a random ECC key pair : {a, A}
     * A = a*G, where G is the base point of the curve
     */

    random_device rd; 
    mt19937 mt(rd());

    uniform_int_distribution<int> dist(1, G.p-1);

    int a = dist(mt);
    EC A = G.mul(a);    

    /* Bob generates a random ECC key pair : {b, B}
     * B = b*G, where G is the base point of the curve
     */

    random_device rd2;
    mt19937 mt2(rd2());

    uniform_int_distribution<int> dist2(1, G.p-1);

    int b = dist2(mt2);
    EC B = G.mul(b);

    /* Alice and Bob exchange their public keys through an unsecure network */

    // Alice calculates the shared key : s = b*A
    EC s = A.mul(b);
    
    // Bob calculates the shared key : s = a*B
    EC s2 = B.mul(a);

    /* Information that Eve (attacker) can get from the unsecure network
     * G, a*G, b*G
     */
    cout<<"Information that an attacker can access"<<endl; 
    G.print(); A.print(); B.print();

    /* Alice and Bob can now use s as the shared secret key
     * check if both the shared keys are same (which they should be)
     */
    assert(s.x == s2.x && s.y == s2.y); 
    return {vector<EC> {s, A, B}, a*b}; 
}


int main()
{ 
    // Elliptic Curve : y^2 = x^3 + 2x + 2 (mod 17)
    EC G = EC(2, 2, 5, 1, 17); 
    auto temp = ecdh_keygen(G); 
    EC s = temp.first[0]; 
    EC A = temp.first[1];
    EC B = temp.first[2];

    int key = temp.second; 

    // Attack : Eve uses the baby step giant step algorithm to find the shared key
    // Eve knows G, a*G, b*G
    // Eve calculates the shared key : s = b*A = b*a*G

    int eve_a = babyStepGiantStep(G, A, G.p);
    int eve_b = babyStepGiantStep(G, B, G.p);

    cout<<"Actual key by encryption : "<<key<<endl;
    cout<<"Key found by Eve : "<<eve_a*eve_b<<endl;
}