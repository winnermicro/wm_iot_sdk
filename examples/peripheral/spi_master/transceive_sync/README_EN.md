# SPI sends and receives data in synchronous mode

## Function Overview

the demo project will do the following two things

1. initialize SPI master
2. Call SPI master synchronous transceiver interface，transmit 256byte data，and receive 256 byte at the same time

## Environmental Requirements

1. The slave device and spi master pins must be connected in advance
2. Start the slave end in advance, wait for receiving and prepare the data to be sent
3. PIN_CS: GPIO20, PIN_CLK: GPIO17, PIN_MISO: GPIO16, PIN_MOSI: GPIO7, mode 0, clock: 2MHz


## Compile and Download

Demo project path：`examples/peripheral/spi_master/transceive_sync`

For compilation, flashing, and other operations, please refer to: [Quick Start Guide](https://doc.winnermicro.net/w800/en/latest/get_started/index.html)

## Running Result

The following log is displayed after successful operation

```
D/dt              [0.002] add device iflash
D/ptb             [0.002] partition table base 0xe000
D/ft              [0.002] ft_0 base 0, ft_1 base 1000
D/ft              [0.004] ft init too
D/ft              [0.004] ft init 1
D/main            [0.004] boot reason 0
D/main            [0.004] heap size 273160(266.76KB)
D/main            [0.004] flash size 0x200000(2MB)
spi master sync transecie start.
D/dt              [0.038] add device spi
rx data as below

[0]00 [1]00 [2]00 [3]00 [4]00 [5]00 [6]00 [7]00 [8]00 [9]00 [10]00 [11]00 [12]00 [13]00 [14]00 [15]00 
[16]00 [17]00 [18]00 [19]00 [20]00 [21]00 [22]00 [23]00 [24]00 [25]00 [26]00 [27]00 [28]00 [29]00 [30]00 [31]00 
[32]00 [33]00 [34]00 [35]00 [36]00 [37]00 [38]00 [39]00 [40]00 [41]00 [42]00 [43]00 [44]00 [45]00 [46]00 [47]00 
[48]00 [49]00 [50]00 [51]00 [52]00 [53]00 [54]00 [55]00 [56]00 [57]00 [58]00 [59]00 [60]00 [61]00 [62]00 [63]00 
[64]00 [65]00 [66]00 [67]00 [68]00 [69]00 [70]00 [71]00 [72]00 [73]00 [74]00 [75]00 [76]00 [77]00 [78]00 [79]00 
[80]00 [81]00 [82]00 [83]00 [84]00 [85]00 [86]00 [87]00 [88]00 [89]00 [90]00 [91]00 [92]00 [93]00 [94]00 [95]00 
[96]00 [97]00 [98]00 [99]00 [100]00 [101]00 [102]00 [103]00 [104]00 [105]00 [106]00 [107]00 [108]00 [109]00 [110]00 [111]00 
[112]00 [113]00 [114]00 [115]00 [116]00 [117]00 [118]00 [119]00 [120]00 [121]00 [122]00 [123]00 [124]00 [125]00 [126]00 [127]00 
[128]00 [129]00 [130]00 [131]00 [132]00 [133]00 [134]00 [135]00 [136]00 [137]00 [138]00 [139]00 [140]00 [141]00 [142]00 [143]00 
[144]00 [145]00 [146]00 [147]00 [148]00 [149]00 [150]00 [151]00 [152]00 [153]00 [154]00 [155]00 [156]00 [157]00 [158]00 [159]00 
[160]00 [161]00 [162]00 [163]00 [164]00 [165]00 [166]00 [167]00 [168]00 [169]00 [170]00 [171]00 [172]00 [173]00 [174]00 [175]00 
[176]00 [177]00 [178]00 [179]00 [180]00 [181]00 [182]00 [183]00 [184]00 [185]00 [186]00 [187]00 [188]00 [189]00 [190]00 [191]00 
[192]00 [193]00 [194]00 [195]00 [196]00 [197]00 [198]00 [199]00 [200]00 [201]00 [202]00 [203]00 [204]00 [205]00 [206]00 [207]00 
[208]00 [209]00 [210]00 [211]00 [212]00 [213]00 [214]00 [215]00 [216]00 [217]00 [218]00 [219]00 [220]00 [221]00 [222]00 [223]00 
[224]00 [225]00 [226]00 [227]00 [228]00 [229]00 [230]00 [231]00 [232]00 [233]00 [234]00 [235]00 [236]00 [237]00 [238]00 [239]00 
[240]00 [241]00 [242]00 [243]00 [244]00 [245]00 [246]00 [247]00 [248]00 [249]00 [250]00 [251]00 [252]00 [253]00 [254]00 [255]00
```
