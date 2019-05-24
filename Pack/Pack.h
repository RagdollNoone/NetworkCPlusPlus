//
// Created by dendy on 19-5-24.
//

#ifndef NETWORK_PACK_H
#define NETWORK_PACK_H

#define pack754_32(f) (pack754((f), 32, 8))
#define pack754_64(f) (pack754((f), 64, 11))
#define unpack754_32(i) (unpack754((i), 32, 8))
#define unpack754_64(i) (unpack754((i), 64, 11))

#endif //NETWORK_PACK_H
