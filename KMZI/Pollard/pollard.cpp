#include <iostream>
#include "big.h"
#include "ecn.h"
#include <vector>


Miracl precision(500, 10); 

std::pair<Big, Big> getRandomPoint(Big p, Big a, Big b) {
    Big xCoordinate, yCoordinate, rightPartEC;
    while (1) {
        xCoordinate = rand(p);
        if (epoint_x(xCoordinate.getbig())) {
            rightPartEC = (pow(xCoordinate, 3, p) + a * xCoordinate + b) % p;
            yCoordinate = sqrt(rightPartEC, p);
            if (yCoordinate != 0) {
                return std::pair<Big, Big> (xCoordinate, yCoordinate);
            }
        }
    }
}

uint8_t H(ECn point) {
    Big x;
    point.getx(x);
    return x % 32;
}

int main() { 

    std::vector<Big> alpha;
    std::vector<Big> beta;
    std::vector<ECn> point;
    /*char pString[] = "1264736038258721544432888496790694991";
    char qString[] = "702040259";
    char xPString[] = "993603374990895673316245812332861613";
    char yPString[] = "264419074466187872158704941697991832";
    char aString[] = "265650828522621884751814088649699757";
    char bString[] = "106166816813909454726076272656867529";*/ //2**30

    /*char pString[] = "801758384607216412550221478044128847";
    char qString[] = "30698370539";
    char xPString[] = "529230763066480266597748892762292160";
    char yPString[] = "550454430014762030706185785134389718";
    char aString[] = "448282689484591754589306602456436183";
    char bString[] = "157602992008573735403998908570163206";*/ // 2**35

    /*char pString[] = "66684469492865428042530053081629757";
    char qString[] = "600913107451";
    char xPString[] = "4001620872114559995122511039547240";
    char yPString[] = "33501921010148193027698067776132652";
    char aString[] = "34221734492264048950945057108949044";
    char bString[] = "28827264964759362976158913883730986"; */// 2**40 примерно минуты 2-3 ищет
    
    /*char pString[] = "20073632066125574435678097206663863";
    char qString[] = "114441376873457";
    char xPString[] = "15171884549307607214453052210953062";
    char yPString[] = "387139961412689035648822185852705";
    char aString[] = "12233931199138854386417127880988847";
    char bString[] = "3924631627418033886371083593449742";. //2**46 посчитало где-то мминут за 20-25*/

    char pString[] = "3294216091";
    char mString[] = "1647090571";
    char xPString[] = "1343026426";
    char yPString[] = "2940538751";
    char xQString[] = "1606805100";
    char yQString[] = "3188929115";

    Big countIter = 0;
    //[7, 14303, 16451]
    Big p(pString), a(11), b(11),
        d(123), q(mString),
        xP(xPString), yP(yPString),
        xQ(xQString), yQ(yQString);

    ecurve(a, b, p, MR_AFFINE);

    const ECn P(xP, yP);
    const ECn Q(xQ, yQ);

    irand(time(NULL));
    for (int i = 0; i < 32; i++) {
        Big randAlpha = rand(q);
        Big randBeta = rand(q);
        ECn randPoint = mul(randAlpha, P, randBeta, Q);
        alpha.push_back(randAlpha);
        beta.push_back(randBeta);
        point.push_back(randPoint);
    }

    Big testAlpha = rand(q);
    Big testBeta = rand(q);
    ECn T = mul(testAlpha, P, testBeta, Q);
    ECn T_ = T;
    Big testAlpha_ = testAlpha;
    Big testBeta_ = testBeta;
    uint8_t j = 0;
    do {
        j = H(T);
        T += point[j];
        testAlpha = (testAlpha + alpha[j]) % q;
        testBeta = (testBeta + beta[j]) % q;

        j = H(T_);
        T_ += point[j];
        testAlpha_ = (testAlpha_ + alpha[j]) % q;
        testBeta_ = (testBeta_ + beta[j]) % q;
        j = H(T_);
        T_ += point[j];
        testAlpha_ = (testAlpha_ + alpha[j]) % q;
        testBeta_ = (testBeta_ + beta[j]) % q;
        countIter += 1;

    } while (T_ != T);

    if (testAlpha == testAlpha_ && testBeta == testBeta_) {
        std::cout << "need choose another L\n";
        return -1;
    }

    Big inv = inverse(testBeta_ - testBeta, q);
    Big razn = (testAlpha - testAlpha_);
    if (razn < 0) {
        razn += q;
    }

    Big dResult = ( razn * inv) % q;
    std::cout << "Result d = " << dResult << std::endl;

    std::cout << "Orig d = " << d << std::endl;

    std::cout << "Count iteration = " << countIter << std::endl;


    alpha.clear();
    beta.clear();
    point.clear();
    return 0;
} 