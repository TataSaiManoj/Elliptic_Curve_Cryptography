#include "ellipticcurve.hpp"
#include <assert.h>
#include <random>

/* Elliptic curve elgamal encryption with random key generation
 *
 * Point P of large order (DLP is hard)
 * Choose random secret integer s
 * Send out P, s*P to the public
 */
pair<int, EC> elgamal_public(EC P)
{
    // Generated secret key of Bob s 
    random_device rd;
    mt19937 mt(rd());

    uniform_int_distribution<int> dist(1, P.p-1);
    int s = dist(mt);

    // Generated public key of Bob s*P
    EC sP = P.mul(s);

    return {s, sP};
}

pair<EC, EC>  elgamal_encryption(EC P, EC sP, EC message)
{
    /* Input for Alice is Bob's public key s*P
     * Alice expresses her message as a point M on the curve
     */

    // Alice generates a random integer k
    random_device rd;
    mt19937 mt(rd());

    uniform_int_distribution<int> dist(1, P.p-1);
    int k = dist(mt);

    // Alice sends out k*P and k*s*P + M to the public
    EC kP = P.mul(k);
    EC kB = sP.mul(k);
    EC modified_message = kB.add(message);

    // Bob receives k*P and k*s*P + M    
    return {kP, modified_message};
}

int main()
{
    // Elliptic curve y^2 = x^3 + 7 over F_23
    EC P(1, 7, 1, 7, 23);

    // Bob's public key
    auto X = elgamal_public(P);
    int s = X.first;
    EC sP = X.second;

    // Alice's message
    EC message(1, 7, 5, 1, 23);

    // Alice sends out k*P and k*s*P + M to the public
    pair<EC, EC> ciphertext = elgamal_encryption(P, sP, message);

    // Bob receives k*P and k*s*P + M
    EC kP = ciphertext.first;
    EC modified_message = ciphertext.second;

    // Bob decrypts the message
    EC decrypted_message = modified_message.sub(kP.mul(s));

    // Bob prints the message
    decrypted_message.print();

    // Bob checks if the message is correct
    assert(decrypted_message.x == message.x);
    assert(decrypted_message.y == message.y);

    return 0;
}