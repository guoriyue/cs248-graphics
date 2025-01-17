#include "lib/newspectrum.h"
// Lambda could be any float between 400nm and 700nm.
// Given a lambda, we need to find the two nearest indexes and do linear 
// interpolation to split c_value and add values to both the left and right index.
void NewSpectrum::addValueAtLambda(float lambda, float c_value) {
    int lambda_index_left = (lambda - Lambda_Start) / (Lambda_End - Lambda_Start) * nSpectrumSamples;
    int lambda_index_right = lambda_index_left + 1;
    float lambda_v_left_side = Lambda_Start + lambda_index_left * Lambda_Interval;
    float t = (lambda - lambda_v_left_side) / Lambda_Interval;
    c[lambda_index_left] += c_value * (1.0f - t);
    c[lambda_index_right] += c_value * t;
}
// Lambda could be any float between 400nm and 700nm.
// Given a lambda, we need to find the two nearest indexes and do linear
// interpolation to split c_value and add values to both left and right indexes.
float NewSpectrum::sampleAtLambda(float lambda) {
    int lambda_index_left = (lambda - Lambda_Start) / (Lambda_End - Lambda_Start) * nSpectrumSamples;
    int lambda_index_right = lambda_index_left + 1;
    float lambda_v_left_side = Lambda_Start + lambda_index_left * Lambda_Interval;
    float t = (lambda - lambda_v_left_side) / Lambda_Interval;
    return c[lambda_index_left] * (1.0f - t) + c[lambda_index_right] * t;
}

NewSpectrum Old2NewSpectrum(Spectrum old_spectrum) {
    return Convert2Spectrum(old_spectrum.r, old_spectrum.g, old_spectrum.b);
}

Spectrum New2OldSpectrum(NewSpectrum new_spectrum) {
    Vec3 old_spectrum = new_spectrum.Convert2RGB();
    return Spectrum(old_spectrum.x, old_spectrum.y, old_spectrum.z);
}
// The length of RGB spectrum samples is 28. This lambda interval doesn't match our spectrum sample array.
// For each lambda value in the array of the NewSpectrum object, we need to find the two nearest indices
// in the lambda array and do linear interpolation to find the intensity.
NewSpectrum::NewSpectrum(std::array<float, RGB2SpectrumSamples>& lambdas,
                         std::array<float, RGB2SpectrumSamples>& values) {
    for(int i = 0; i < nSpectrumSamples; ++i) {
        float lambda_at_index = Lambda_Start + i * Lambda_Interval;
        int lambda_index_left =
            std::lower_bound(lambdas.begin(), lambdas.end(), lambda_at_index) - lambdas.begin();
        int lambda_index_right = lambda_index_left + 1;
        float Lambda_Interval = lambdas[lambda_index_right] - lambdas[lambda_index_left];
        float t = (lambda_at_index - lambdas[lambda_index_left]) / Lambda_Interval;
        c[i] = values[lambda_index_left] * (1.0f - t) + values[lambda_index_right] * t;
    }
}

// Convert spectrum to XYZ, then to RGB
// XYZ value can be computed by integrating the spectrum with the spectral matching curves
// CIE_X, CIE_Y, and CIE_Z.
Vec3 NewSpectrum::Convert2RGB() {
    //Get CIE_X, CIE_Y, CIE_Z spectral matching curves from
    //https://github.com/hughsie/colord/blob/main/data/cmf/CIE1931-2deg-XYZ.csv
    std::array<float, nCIESamples> CIE_X = {
        0.014310000000f, 0.023190000000f, 0.043510000000f, 0.077630000000f, 0.134380000000f,
        0.214770000000f, 0.283900000000f, 0.328500000000f, 0.348280000000f, 0.348060000000f,
        0.336200000000f, 0.318700000000f, 0.290800000000f, 0.251100000000f, 0.195360000000f,
        0.142100000000f, 0.095640000000f, 0.057950010000f, 0.032010000000f, 0.014700000000f,
        0.004900000000f, 0.002400000000f, 0.009300000000f, 0.029100000000f, 0.063270000000f,
        0.109600000000f, 0.165500000000f, 0.225749900000f, 0.290400000000f, 0.359700000000f,
        0.433449900000f, 0.512050100000f, 0.594500000000f, 0.678400000000f, 0.762100000000f,
        0.842500000000f, 0.916300000000f, 0.978600000000f, 1.026300000000f, 1.056700000000f,
        1.062200000000f, 1.045600000000f, 1.002600000000f, 0.938400000000f, 0.854449900000f,
        0.751400000000f, 0.642400000000f, 0.541900000000f, 0.447900000000f, 0.360800000000f,
        0.283500000000f, 0.218700000000f, 0.164900000000f, 0.121200000000f, 0.087400000000f,
        0.063600000000f, 0.046770000000f, 0.032900000000f, 0.022700000000f, 0.015840000000f};
    std::array<float, nCIESamples> CIE_Y = {
        0.000396000000f, 0.000640000000f, 0.001210000000f, 0.002180000000f, 0.004000000000f,
        0.007300000000f, 0.011600000000f, 0.016840000000f, 0.023000000000f, 0.029800000000f,
        0.038000000000f, 0.048000000000f, 0.060000000000f, 0.073900000000f, 0.090980000000f,
        0.112600000000f, 0.139020000000f, 0.169300000000f, 0.208020000000f, 0.258600000000f,
        0.323000000000f, 0.407300000000f, 0.503000000000f, 0.608200000000f, 0.710000000000f,
        0.793200000000f, 0.862000000000f, 0.914850100000f, 0.954000000000f, 0.980300000000f,
        0.994950100000f, 1.000000000000f, 0.995000000000f, 0.978600000000f, 0.952000000000f,
        0.915400000000f, 0.870000000000f, 0.816300000000f, 0.757000000000f, 0.694900000000f,
        0.631000000000f, 0.566800000000f, 0.503000000000f, 0.441200000000f, 0.381000000000f,
        0.321000000000f, 0.265000000000f, 0.217000000000f, 0.175000000000f, 0.138200000000f,
        0.107000000000f, 0.081600000000f, 0.061000000000f, 0.044580000000f, 0.032000000000f,
        0.023200000000f, 0.017000000000f, 0.011920000000f, 0.008210000000f, 0.005723000000f};
    std::array<float, nCIESamples> CIE_Z = {
        0.067850010000f, 0.110200000000f, 0.207400000000f, 0.371300000000f, 0.645600000000f,
        1.039050100000f, 1.385600000000f, 1.622960000000f, 1.747060000000f, 1.782600000000f,
        1.772110000000f, 1.744100000000f, 1.669200000000f, 1.528100000000f, 1.287640000000f,
        1.041900000000f, 0.812950100000f, 0.616200000000f, 0.465180000000f, 0.353300000000f,
        0.272000000000f, 0.212300000000f, 0.158200000000f, 0.111700000000f, 0.078249990000f,
        0.057250010000f, 0.042160000000f, 0.029840000000f, 0.020300000000f, 0.013400000000f,
        0.008749999000f, 0.005749999000f, 0.003900000000f, 0.002749999000f, 0.002100000000f,
        0.001800000000f, 0.001650001000f, 0.001400000000f, 0.001100000000f, 0.001000000000f,
        0.000800000000f, 0.000600000000f, 0.000340000000f, 0.000240000000f, 0.000190000000f,
        0.000100000000f, 0.000049999990f, 0.000030000000f, 0.000020000000f, 0.000010000000f,
        0.000000000000f, 0.000000000000f, 0.000000000000f, 0.000000000000f, 0.000000000000f,
        0.000000000000f, 0.000000000000f, 0.000000000000f, 0.000000000000f, 0.000000000000f};

    Vec3 xyz;
    // Since the lambda interval of CIE_X, CIE_Y, and CIE_Z matches the 
    // lambda interval of our spectrum sample array. The integration is 
    // straightforward, and we don't need linear interpolation.
    for(int i = 0; i < nSpectrumSamples; ++i) {
        float c_value = c[i];
        xyz[0] += CIE_X[i] * c_value;
        xyz[1] += CIE_Y[i] * c_value;
        xyz[2] += CIE_Z[i] * c_value;
    }
    // XYZ To RGB. Matrix value gets from
    // http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
    return Vec3(3.240479f * xyz.x - 1.537150f * xyz.y - 0.498535f * xyz.z,
                -0.969256f * xyz.x + 1.875991f * xyz.y + 0.041556f * xyz.z,
                0.055648f * xyz.x - 0.204043f * xyz.y + 1.057311f * xyz.z);
}

// Using Smits' method to convert an RGB value to a spectrum
// Smits, B. 1999. An RGB-to-spectrum conversion for reflectances. Journal of Graphics Tools 4 (4), 11–22.
NewSpectrum Convert2Spectrum(float r, float g, float b) {
    // All spectrum samples arrays get from 
    // https://github.com/mmp/pbrt-v3/blob/master/src/core/spectrum.cpp
    std::array<float, RGB2SpectrumSamples> sampled_lambda = {
        401.935486, 412.903229, 423.870972, 434.838715, 445.806458, 456.774200, 467.741943,
        478.709686, 489.677429, 500.645172, 511.612915, 522.580627, 533.548340, 544.516052,
        555.483765, 566.451477, 577.419189, 588.386902, 599.354614, 610.322327, 621.290039,
        632.257751, 643.225464, 654.193176, 665.160889, 676.128601, 687.096313, 698.064026};

    std::array<float, RGB2SpectrumSamples> white_spectrum_arr = {
        1.0614335379927147e+00, 1.0622711654692485e+00, 1.0622036218416742e+00,
        1.0625059965187085e+00, 1.0623938486985884e+00, 1.0624706448043137e+00,
        1.0625048144827762e+00, 1.0624366131308856e+00, 1.0620694238892607e+00,
        1.0613167586932164e+00, 1.0610334029377020e+00, 1.0613868564828413e+00,
        1.0614215366116762e+00, 1.0620336151299086e+00, 1.0625497454805051e+00,
        1.0624317487992085e+00, 1.0625249140554480e+00, 1.0624277664486914e+00,
        1.0624749854090769e+00, 1.0625538581025402e+00, 1.0625326910104864e+00,
        1.0623922312225325e+00, 1.0623650980354129e+00, 1.0625256476715284e+00,
        1.0612277619533155e+00, 1.0594262608698046e+00, 1.0599810758292072e+00,
        1.0602547314449409e+00,
    };

    std::array<float, RGB2SpectrumSamples> cyan_spectrum_arr = {
        1.0126146228964314e+00,  1.0350460524836209e+00,  1.0078661447098567e+00,
        1.0422280385081280e+00,  1.0442596738499825e+00,  1.0535238290294409e+00,
        1.0180776226938120e+00,  1.0442729908727713e+00,  1.0529362541920750e+00,
        1.0537034271160244e+00,  1.0533901869215969e+00,  1.0537782700979574e+00,
        1.0527093770467102e+00,  1.0530449040446797e+00,  1.0550554640191208e+00,
        1.0553673610724821e+00,  1.0454306634683976e+00,  6.2348950639230805e-01,
        1.8038071613188977e-01,  -7.6303759201984539e-03, -1.5217847035781367e-04,
        -7.5102257347258311e-03, -2.1708639328491472e-03, 6.5919466602369636e-04,
        1.2278815318539780e-02,  -4.4669775637208031e-03, 1.7119799082865147e-02,
        4.9211089759759801e-03,
    };

    std::array<float, RGB2SpectrumSamples> magenta_spectrum_arr = {
        9.8293658286116958e-01,  9.9627868399859310e-01,  1.0198955019000133e+00,
        1.0166395501210359e+00,  1.0220913178757398e+00,  9.9651666040682441e-01,
        1.0097766178917882e+00,  1.0215422470827016e+00,  6.4031953387790963e-01,
        2.5012379477078184e-03,  6.5339939555769944e-03,  2.8334080462675826e-03,
        -5.1209675389074505e-11, -9.0592291646646381e-03, 3.3936718323331200e-03,
        -3.0638741121828406e-03, 2.2203936168286292e-01,  6.3141140024811970e-01,
        9.7480985576500956e-01,  9.7209562333590571e-01,  1.0173770302868150e+00,
        9.9875194322734129e-01,  9.4701725739602238e-01,  8.5258623154354796e-01,
        9.4897798581660842e-01,  9.4751876096521492e-01,  9.9598944191059791e-01,
        8.6301351503809076e-01,
    };

    std::array<float, RGB2SpectrumSamples> yellow_spectrum_arr = {
        -5.2536564298613798e-03, -6.4571480044499710e-03, -5.9693514658007013e-03,
        -2.1836716037686721e-03, 1.6781120601055327e-02,  9.6096355429062641e-02,
        2.1217357081986446e-01,  3.6169133290685068e-01,  5.3961011543232529e-01,
        7.4408810492171507e-01,  9.2209571148394054e-01,  1.0460304298411225e+00,
        1.0513824989063714e+00,  1.0511991822135085e+00,  1.0510530911991052e+00,
        1.0517397230360510e+00,  1.0516043086790485e+00,  1.0511944032061460e+00,
        1.0511590325868068e+00,  1.0516612465483031e+00,  1.0514038526836869e+00,
        1.0515941029228475e+00,  1.0511460436960840e+00,  1.0515123758830476e+00,
        1.0508871369510702e+00,  1.0508923708102380e+00,  1.0477492815668303e+00,
        1.0493272144017338e+00,
    };

    std::array<float, RGB2SpectrumSamples> red_spectrum_arr = {
        1.2408293329637447e-01,  1.1371272058349924e-01,  7.8992434518899132e-02,
        3.2205603593106549e-02,  -1.0798365407877875e-02, 1.8051975516730392e-02,
        5.3407196598730527e-03,  1.3654918729501336e-02,  -5.9564213545642841e-03,
        -1.8444365067353252e-03, -1.0571884361529504e-02, -2.9375521078000011e-03,
        -1.0790476271835936e-02, -8.0224306697503633e-03, -2.2669167702495940e-03,
        7.0200240494706634e-03,  -8.1528469000299308e-03, 6.0772866969252792e-01,
        9.8831560865432400e-01,  9.9391691044078823e-01,  1.0039338994753197e+00,
        9.9234499861167125e-01,  9.9926530858855522e-01,  1.0084621557617270e+00,
        9.8358296827441216e-01,  1.0085023660099048e+00,  9.7451138326568698e-01,
        9.8543269570059944e-01,
    };

    std::array<float, RGB2SpectrumSamples> green_spectrum_arr = {
        -1.2547236272489583e-02, -9.4554964308388671e-03, -1.2526086181600525e-02,
        -7.9170697760437767e-03, -7.9955735204175690e-03, -9.3559433444469070e-03,
        6.5468611982999303e-02,  3.9572875517634137e-01,  7.5244022299886659e-01,
        9.6376478690218559e-01,  9.9854433855162328e-01,  9.9992977025287921e-01,
        9.9939086751140449e-01,  9.9994372267071396e-01,  9.9939121813418674e-01,
        9.9911237310424483e-01,  9.6019584878271580e-01,  6.3186279338432438e-01,
        2.5797401028763473e-01,  9.4014888527335638e-03,  -3.0798345608649747e-03,
        -4.5230367033685034e-03, -6.8933410388274038e-03, -9.0352195539015398e-03,
        -8.5913667165340209e-03, -8.3690869120289398e-03, -7.8685832338754313e-03,
        -8.3657578711085132e-06,
    };

    std::array<float, RGB2SpectrumSamples> blue_spectrum_arr = {
        9.9539040744505636e-01,  9.9529317353008218e-01,  9.9181447411633950e-01,
        1.0002584039673432e+00,  9.9968478437342512e-01,  9.9988120766657174e-01,
        9.8504012146370434e-01,  7.9029849053031276e-01,  5.6082198617463974e-01,
        3.3133458513996528e-01,  1.3692410840839175e-01,  1.8914906559664151e-02,
        -5.1129770932550889e-06, -4.2395493167891873e-04, -4.1934593101534273e-04,
        1.7473028136486615e-03,  3.7999160177631316e-03,  -5.5101474906588642e-04,
        -4.3716662898480967e-05, 7.5874501748732798e-03,  2.5795650780554021e-02,
        3.8168376532500548e-02,  4.9489586408030833e-02,  4.9595992290102905e-02,
        4.9814819505812249e-02,  3.9840911064978023e-02,  3.0501024937233868e-02,
        2.1243054765241080e-02,
    };

    NewSpectrum white_spectrum = NewSpectrum(sampled_lambda, white_spectrum_arr);
    NewSpectrum cyan_spectrum = NewSpectrum(sampled_lambda, cyan_spectrum_arr);
    NewSpectrum magenta_spectrum = NewSpectrum(sampled_lambda, magenta_spectrum_arr);
    NewSpectrum yellow_spectrum = NewSpectrum(sampled_lambda, yellow_spectrum_arr);
    NewSpectrum red_spectrum = NewSpectrum(sampled_lambda, red_spectrum_arr);
    NewSpectrum green_spectrum = NewSpectrum(sampled_lambda, green_spectrum_arr);
    NewSpectrum blue_spectrum = NewSpectrum(sampled_lambda, blue_spectrum_arr);
    NewSpectrum ret;

    if(r <= g && r <= b) {
        ret += r * white_spectrum;
        if(g <= b) {
            ret += (g - r) * cyan_spectrum;
            ret += (b - g) * blue_spectrum;
        } else {
            ret += (b - r) * cyan_spectrum;
            ret += (g - b) * green_spectrum;
        }
    } else if(g <= r && g <= b) {
        ret += g * white_spectrum;
        if(r <= b) {
            ret += (r - g) * magenta_spectrum;
            ret += (b - r) * blue_spectrum;
        } else {
            ret += (b - g) * magenta_spectrum;
            ret += (r - b) * red_spectrum;
        }
    } else {
        ret += b * white_spectrum;
        if(r <= g) {
            ret += (r - b) * yellow_spectrum;
            ret += (g - r) * green_spectrum;
        } else {
            ret += (g - b) * yellow_spectrum;
            ret += (r - g) * red_spectrum;
        }
    }
    return ret;
}