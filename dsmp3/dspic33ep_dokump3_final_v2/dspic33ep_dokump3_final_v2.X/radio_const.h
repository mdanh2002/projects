#ifndef __RADIO_CONST__
#define __RADIO_CONST__

// Radio Defines
#define RADIO_CUR_BAND_FM       0
#define RADIO_CUR_BAND_MW       1
#define RADIO_CUR_BAND_SW       2
#define RADIO_CUR_BAND_LW       3
#define MAX_RADIO_BANDS         4

#define RADIO_MW_SPACING_9kHZ        0
#define RADIO_MW_SPACING_9kHZ_VAL    9
#define RADIO_MW_SPACING_10kHZ       1
#define RADIO_MW_SPACING_10kHZ_VAL   10

#define RADIO_AM_BW_6kHz        0
#define RADIO_AM_BW_6kHz_VAL    "6"
#define RADIO_AM_BW_4kHz        1
#define RADIO_AM_BW_4kHz_VAL    "4"
#define RADIO_AM_BW_3kHz        2
#define RADIO_AM_BW_3kHz_VAL    "3"
#define RADIO_AM_BW_2kHz        3
#define RADIO_AM_BW_2kHz_VAL    "2"
#define RADIO_AM_BW_1kHz        4
#define RADIO_AM_BW_1kHz_VAL    "1"
#define RADIO_AM_BW_1P8kHz      5
#define RADIO_AM_BW_1P8kHz_VAL  "1.8"
#define RADIO_AM_BW_2P5kHz      6
#define RADIO_AM_BW_2P5kHz_VAL  "2.5"
#define MAX_RADIO_AM_BW         7

#define RADIO_SSB_BW_1P2kHz         0
#define RADIO_SSB_BW_1P2kHz_VAL     "1.2"
#define RADIO_SSB_BW_2P2kHz         1
#define RADIO_SSB_BW_2P2kHz_VAL     "2.2"
#define RADIO_SSB_BW_3kHz           2
#define RADIO_SSB_BW_3kHz_VAL       "3"
#define RADIO_SSB_BW_4kHz           3
#define RADIO_SSB_BW_4kHz_VAL       "4"
#define RADIO_SSB_BW_500Hz          4
#define RADIO_SSB_BW_500Hz_VAL      "0.5"
#define MAX_RADIO_SSB_BW            5

#define RADIO_FM_SPACING_100kHZ        0
#define RADIO_FM_SPACING_100kHZ_VAL    10
#define RADIO_FM_SPACING_200kHZ        1
#define RADIO_FM_SPACING_200kHZ_VAL    20
#define RADIO_FM_SPACING_50kHZ         2
#define RADIO_FM_SPACING_50kHZ_VAL     5
#define MAX_FM_SPACING                 3

#define RADIO_FM_DEEMPH_50US           0
#define RADIO_FM_DEEMPH_75US           1

#define RADIO_FM_DEEMPH_50US_VAL           50
#define RADIO_FM_DEEMPH_75US_VAL           75

#define RADIO_FM_RANGE_87MHz           0
#define RADIO_FM_RANGE_76MHz           1
#define RADIO_FM_RANGE_64MHz           2
#define MAX_RADIO_FM_RANGES            3

#define RADIO_FM_MIN_87MHz             (8700)           // in 0.1MHz
#define RADIO_FM_MIN_76MHz             (7600)
#define RADIO_FM_MIN_64MHz             (6400)
#define RADIO_FM_MAX_MHz               (10800)

#define RADIO_AM_MIN_9kHz              (522)            // in kHz
#define RADIO_AM_MIN_10kHz             (520)
#define RADIO_AM_MAX_kHz               (1710)

#define RADIO_SW_MIN_kHz               (1711)           // in kHz
#define RADIO_SW_MAX_kHz               (30000)
#define RADIO_SW_SPACING               (5)

#define RADIO_LW_MIN_kHz               (153)
#define RADIO_LW_MAX_kHz               (279)            // SI4735 limitations, other radio supports long wave from 150-450
#define RADIO_LW_SPACING               (1)

// commercial shortwave band limits
__prog__ static const unsigned int __attribute__((section("swBandsConst"), space(prog))) swBands[] =  {
2300, 2495, 
3200, 3400, 
3900, 4000, 
4750, 4995, 
5900, 6200, 
7200, 7450, 
9400, 10100, 
11600, 12100, 
13570, 13870, 
15100, 15800, 
17480, 17900, 
18900, 19020, 
21450, 21850, 
25670, 26100 
};

// self-test frequencies at bootup
// should be valid within intended AM/FM ranges
#define RADIO_TEST_AM_FREQ            522
#define RADIO_TEST_FM_FREQ            6400

// marker for invalid frequency
#define RADIO_INVALID_FREQUENCY       0xFFFF

#endif