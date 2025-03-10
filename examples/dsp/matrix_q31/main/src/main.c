#define LOG_TAG "test"
#include "wm_log.h"

#include "csi_math.h"

#define MAT_ROW  16
#define MAT_COL  16
#define MAT_SIZE 16 * 16

q31_t data_src0[MAT_SIZE] = {
    0x3e6386a, 0x5f9e7e7, 0x7b63e96, 0x8d5e856, 0x73be7bf, 0x1248208, 0xd9b2873, 0x692a55c, 0x06eb1b7, 0xe24827e, 0x8d6fb14,
    0x7d92249, 0x92e74d1, 0xf9d3352, 0x0c4a540, 0x7e4bb61, 0xcd4cd29, 0x91fa08c, 0x0378df1, 0x2eed447, 0x79d190d, 0x7b5293b,
    0xfa7ecfd, 0x629dc7d, 0x9e6d2b1, 0xc29cd59, 0x4d19a1c, 0xb9fe299, 0x9ed99f8, 0xde49a1d, 0x111dca9, 0xdb51116, 0x96f0cec,
    0x7ccf56b, 0x7e1a5dc, 0x1bca8dd, 0x87ea03e, 0x5984c2c, 0x166011b, 0x4b6a366, 0xe1510d0, 0x60a1d7f, 0x2db6aca, 0xe5e3aaf,
    0x7b49e39, 0x27d620e, 0x2e2e5db, 0x40bb4a9, 0x5372794, 0x6c36f28, 0xf4b22c4, 0xb83b5f1, 0xfe6f0a5, 0xc88fcc5, 0xa55385a,
    0x6d01df1, 0x5015caf, 0x4e5c4e6, 0x722da46, 0x3252535, 0xff2e3f3, 0x702de6d, 0xdf12aa9, 0x9c2c236, 0xe3bbf96, 0x1073675,
    0xed1103e, 0xe801f65, 0x5cdb5b7, 0x2eb58ba, 0x67c3bf2, 0xadc8747, 0xbe7d11e, 0xf60c07f, 0x97705ab, 0x908c4bf, 0x85bce6b,
    0x1ed42f5, 0x9406cbf, 0x18d7f4c, 0x467a60f, 0xee744c1, 0xb6953e6, 0x2141a5a, 0xbe2d21e, 0xdbae0c9, 0x23e3c53, 0x047d93a,
    0x626985f, 0x974943e, 0x688f6ac, 0x3f66bfd, 0x8dc7e78, 0xc868354, 0xf4d366a, 0xee3b7ca, 0xadeb220, 0xd67cfb3, 0xb178cc4,
    0x0e28a3f, 0x5592bab, 0x7ca73b9, 0xa9b6686, 0x20836ba, 0x3670053, 0x1073ebf, 0x028c484, 0xeb9f5cd, 0xa4104e2, 0xb486c22,
    0x8c3f7dc, 0x3b10560, 0x9bff18a, 0x388154a, 0x552f61d, 0xa0a9057, 0x5047832, 0x9580203, 0x10df843, 0x69c6517, 0xe0ce560,
    0xaff8533, 0x1d6a51e, 0x4fb27f9, 0x64babb8, 0xf06b175, 0x3cfe690, 0xc2dc710, 0x67b874f, 0x837eee6, 0x3a04e39, 0xab5094f,
    0x210461a, 0xc0140be, 0x6e7e652, 0x29f9d2d, 0xbfb9321, 0x081ccf0, 0xd0ea0ac, 0xe12dcfe, 0xf73ffbf, 0x86f9ded, 0x89ba77f,
    0x984550f, 0x60588a1, 0xdfc268a, 0x079d69e, 0x80417f7, 0x8092aeb, 0x4fb4696, 0x48795d7, 0xe3a2240, 0xc0bb05e, 0xdae376b,
    0x93d6383, 0xd1ae6db, 0x0f944ea, 0x604d9dd, 0x74495b2, 0xf37257d, 0x7ff267c, 0x6c35193, 0x543ea29, 0xeb0cb21, 0xe135968,
    0x959cc0a, 0x2f89313, 0x3d71230, 0x446a138, 0xd095a6d, 0x59c1d7e, 0x7c6dd7b, 0xfe94b0f, 0x1c2f016, 0x3e0c1d0, 0x23dbec5,
    0x5d3fcb8, 0x0be6171, 0x030a33e, 0xfd0d65b, 0x678721b, 0x0800428, 0xf75a25a, 0x515084a, 0x8cc3bf9, 0x3a0bba9, 0xa4ffb72,
    0x37dab42, 0x915a63c, 0xb41ea15, 0x6ffcb1e, 0xa1cd4bc, 0x8970ce2, 0xf950a66, 0x0d4d5f1, 0xa8ed2a1, 0x2e2b86d, 0x4cf5a0a,
    0xc2a434e, 0x19b0324, 0x2f6f125, 0x0409b0a, 0xded8282, 0xe9c6627, 0x65cc29e, 0x051944d, 0x036bd27, 0xf3ec2bb, 0x0b704ef,
    0xff1d11d, 0xb699d6b, 0x2ab5eed, 0xe0fa8d7, 0x3d23851, 0x11bb8f1, 0x95c25e1, 0x1eca8c0, 0x4a67b43, 0x1f550b8, 0x993fd4c,
    0xc9df6a8, 0x8dc76df, 0xbdabefd, 0x26f68d8, 0x7f63c62, 0xcf18ba9, 0x167b699, 0x84966de, 0x21c7c8a, 0xa444066, 0xbfaf6f2,
    0x6c1fa76, 0x3f26b95, 0xb459a03, 0x7f5f8fb, 0x3c0c111, 0xc4e0408, 0x5cb850c, 0xc28d15a, 0x53b629a, 0x02d16e2, 0x736a171,
    0xa48c217, 0x9ea30b4, 0xbe46e95, 0xf019d68, 0xd1bdbfc, 0x8db8962, 0x7a2d6d1, 0x2d985c2, 0x0470aa4, 0x2326814, 0x181d1c0,
    0xf1642a9, 0x424c217, 0xcede20d,
};

q31_t data_src1[MAT_SIZE] = {
    0xe7014cad, 0xfabb35de, 0x6a5caca5, 0x3aa21c61, 0x0434ea19, 0x356daa70, 0x003d994e, 0x5ef41f8b, 0xbfbb5627, 0x3d063e39,
    0xd8bdf7e0, 0x83c78d29, 0xd46c70ba, 0x055674ed, 0x5d795568, 0x49f050a0, 0x625cfbc9, 0x10a3957e, 0x6545d05e, 0xb3dea603,
    0x81ec0157, 0x7e308eef, 0xd4fdc98d, 0x390c46b0, 0xe2ec9acb, 0x79f7e096, 0xd5a99db7, 0x33e864e8, 0xb448c9f4, 0xb8157431,
    0x916a4352, 0xf918893d, 0x33592dc4, 0xfd57b242, 0xfb858dbe, 0xb75de09d, 0x305defda, 0xa9980f16, 0xac839aff, 0x2b07adfb,
    0xee726af2, 0x05f6ed55, 0x131a1233, 0x01ba2e6b, 0x44682f5a, 0x57a6bbd2, 0x77f2a072, 0x542c591e, 0xbdeb57bd, 0xc512600a,
    0xf157683d, 0x79f10f0b, 0x722e642e, 0x9d12aeff, 0x2029b4ef, 0xadc7a880, 0x54b4fa07, 0xee0eede1, 0x5d2b046a, 0xe19aa7b0,
    0x91e9c2a2, 0x29b5f89c, 0x994817fa, 0xd28e2742, 0x4284567f, 0x7d5f9c26, 0xcfcf2c2c, 0x17de4d07, 0x5fa015d0, 0x69b23434,
    0x133c0952, 0x0ced437a, 0x531c0dd1, 0xb5084fe6, 0xb2d5078d, 0x909f5a63, 0x73356cc1, 0x50f4cbb0, 0x0c06b0ba, 0x79e4bd45,
    0xca7a1bef, 0xaf055ed5, 0x8e290b1a, 0xcded6495, 0x9cffaf83, 0xfb4ddf33, 0x40409402, 0x75ba5f87, 0xf3f7b791, 0xd2ca6ecc,
    0x2c25bf11, 0xdbcbe574, 0xa8806fbc, 0x4b3b8ff9, 0xe7290934, 0xc738d6b7, 0xc7063951, 0x5c9588fe, 0x40f87f67, 0x77bb604c,
    0xdac50e6d, 0x5a0ff472, 0xa74d0210, 0x1894e055, 0xe16c642c, 0x9c61fd9a, 0x66de6bb5, 0xbbf7e3b2, 0xc954842c, 0xf8174754,
    0x9b66fbcf, 0x92a50a25, 0x819047ee, 0x885a9bb6, 0xa1c74f5e, 0x655e4761, 0xbdef9277, 0x4f5682d1, 0xdb5667dd, 0x4efe7d6f,
    0x6d05b888, 0xe00dc420, 0xb2fbf70e, 0xb418d774, 0x2fe7b194, 0xcf3d0770, 0x3962f443, 0x405030b3, 0xdfed11c1, 0xd4fb3366,
    0xdb1db645, 0xb0a58278, 0x0f7200f9, 0xafcf7937, 0xa4d9f5ab, 0x7c0a4ffa, 0x3da7ed8f, 0xd4749651, 0xcc5c599c, 0x5053dbd8,
    0xa4226514, 0x3005318d, 0x1d1a494f, 0x54c8c706, 0xefdad3a4, 0x3fade214, 0xe557ccac, 0x8075eba5, 0x1cdbaf08, 0xbf487d64,
    0x59c16c5d, 0x62cbf1d1, 0x3cd58567, 0xd794345e, 0xff1bf3d3, 0xe4b82129, 0x03181c59, 0x7ca258e9, 0x48709a7d, 0x6c1e596b,
    0xcde68124, 0x24f53341, 0x62b184e8, 0x36362666, 0xcd01d317, 0x8ddf51e5, 0xd67ea19d, 0xb6be0597, 0x72691640, 0x512eb377,
    0x63d0c384, 0x8db71a37, 0x38a8e1aa, 0x451a62f1, 0x110e0be7, 0xd3b7dd35, 0xca75d298, 0xab532ddb, 0x856ff47f, 0x5e2357c4,
    0x4c5303f0, 0x1be4e5e4, 0xc6734701, 0x88dd8405, 0x0295e4f1, 0x081c875c, 0x005cb4e1, 0xe0036157, 0x6dc8cb79, 0x545f690e,
    0x4fb29cc5, 0x4dd77bb2, 0xbe158c7f, 0x73c3b6a7, 0x5815ac8c, 0x9e496d5c, 0x4baf2b6e, 0x46f8b459, 0x8189ef65, 0xf37cd94b,
    0x4ab89822, 0x0568412c, 0xc6e928f5, 0x466604b7, 0x3b6b286d, 0x34c1f8a0, 0x13a79531, 0x09cac61b, 0x6fca7f40, 0x0b13c216,
    0xc9beff6a, 0x89fd68e9, 0x47f5a9a9, 0x02d5189d, 0x4d4a1a4f, 0x83880a42, 0xf3c1a485, 0x4639293d, 0x08b17bb3, 0xaa50f1e4,
    0x3ff60a36, 0x1867f7b0, 0x71abe5b2, 0xf69a7e42, 0x5c356d79, 0xc05ca232, 0xc016789d, 0x192346c3, 0xd9e1963d, 0x871aa13a,
    0xff56263c, 0xf9451b8f, 0x59651625, 0x9ec98a7b, 0x130204cd, 0x69872d7a, 0xe84618f6, 0x40bc5bbc, 0x5f177cf0, 0x521cb03e,
    0xe5b0c054, 0x141d509d, 0xfd0672a0, 0x1ab44a37, 0x8de66e6f, 0x7d89e3cb, 0x09b01186, 0x7381a70d, 0xe3f3465c, 0x201d804c,
    0xe9ad8c9d, 0xd1b7785a, 0xbd4f53e8, 0xff292fb5, 0x01f237e3, 0x73b02511,
};

q31_t ref_result[MAT_SIZE] = {
    0xf84cf2b8, 0x1536ab68, 0x0061195f, 0xfca81fb9, 0x0d8c7a53, 0x0dec5e3b, 0xf5b41d95, 0x00e59e7f, 0x1a13101c, 0x01a576bf,
    0x09c49345, 0xd6ada1e7, 0x0c86111d, 0x25d80f60, 0x14aa5962, 0x170e1386, 0xf0f7277b, 0x0facf024, 0xfe83bde7, 0xfc111eca,
    0xfc37a520, 0x24e81ef3, 0xf040feca, 0x1dae7005, 0x106d63a3, 0x00c1d77f, 0xfc26b982, 0xd645e218, 0xfa88c2d8, 0x238eda3f,
    0x14e584dd, 0x229fad0f, 0xf73fceba, 0xfd06a467, 0xf471a945, 0xf7900747, 0x08503d1d, 0x09f5056a, 0xe775f417, 0x18d09235,
    0x141df628, 0xff60a630, 0xf1106a9c, 0xf216269b, 0x05af82e7, 0x24038a0e, 0x1c6a2b02, 0x2c7827f9, 0xfc079d2f, 0x0e2a1cac,
    0xf7c4f82d, 0xfd449c9a, 0x067b85a1, 0x07e663c3, 0xe782b934, 0x19450c43, 0x294479db, 0xee67427d, 0x0304fab1, 0xe738de98,
    0x01fe89f6, 0x344db25a, 0x170d4b73, 0x2629a60f, 0xe7f74143, 0xfab77a1d, 0xf94564df, 0x07db2ce5, 0x14fb02da, 0xe97bf82b,
    0xe88aeab7, 0x19b55658, 0x2be1c0f4, 0xf2ad8359, 0x043f4db4, 0xe374b9b4, 0x0099d99e, 0x36b8b1c8, 0x27b15277, 0x2fd76638,
    0x14a583e6, 0x0a9c6ade, 0xf55dafb8, 0xe0121d67, 0xf22de778, 0x0a077139, 0xf9543d5e, 0x2042689d, 0x191eb6fd, 0x0197d477,
    0xfb45ee66, 0xf095bf67, 0xfc56d6aa, 0x2efad848, 0x1e58f161, 0x2f33a504, 0x07e3ca2a, 0x02e27143, 0xfcbbda44, 0xf90fb8bb,
    0x01d5a9af, 0x18209af4, 0xe5d3e3e9, 0x09628ecd, 0x05492aa2, 0x042596a8, 0xfceed6b0, 0xef330357, 0x066391aa, 0x1e31ea2d,
    0x1a506d4e, 0x1b121669, 0xf7e03a16, 0xf9e8a6ff, 0xed1d7f79, 0xedd0db2b, 0x07346730, 0x02165c02, 0xff541fa3, 0x1d822d7b,
    0x17f22337, 0x000b151d, 0xf95ec677, 0xe5e6026f, 0xf399bf33, 0x26fc6384, 0x1fe2eadd, 0x2a40af9f, 0xecf46eb9, 0xfb5f785b,
    0xfc3a9f77, 0x027fa05e, 0xff8434d1, 0x01743688, 0xe2237bdf, 0x06dc8386, 0x205f0c32, 0x042e03fd, 0x092defb6, 0xeee00701,
    0xf7712120, 0x2a24dd5b, 0x0ff86daf, 0x150940a5, 0xf0ab2782, 0xf5809187, 0xede587a2, 0x079f5995, 0xf0257e93, 0x1120c0bd,
    0xf56c7863, 0x20e6a923, 0x2562e598, 0x0177958f, 0xf8131c3e, 0xe0ccfab1, 0xf774ac44, 0x223bddc1, 0x13eeba7f, 0x33444b3d,
    0xeff739fc, 0x0e993e2f, 0xfc5754a5, 0xfb594b61, 0x163c5888, 0x05aabf1d, 0xf3ec9689, 0x108cb049, 0x27089a02, 0xf75c5b20,
    0xfd64fcb9, 0xe7ba2bb1, 0x02ae58e8, 0x30f8b69a, 0x0ce60bfa, 0x2161ef2c, 0xee8e1ccb, 0x0bd7307d, 0x03e8273a, 0x14b8452b,
    0x0b6bc589, 0x02fae097, 0xee953972, 0x00e02004, 0x2248a558, 0xf66ae246, 0x0f9c419d, 0xe02205e3, 0xf5770a0b, 0x1eab85bd,
    0x0615fb78, 0x0e1154d9, 0xeb5a27f3, 0x056ea40d, 0x117b1f5b, 0x1c30094d, 0xee99d7e7, 0x1d0d8e17, 0xe4e39e5d, 0x092b35f2,
    0x0db00686, 0x1105b2f9, 0x0d030c67, 0xdfde0a2b, 0xf1a4520a, 0x1419f876, 0xf508dd03, 0x0c922ec4, 0x0eb917cf, 0x068c4417,
    0xf34053fd, 0xef91c79d, 0x065c4b00, 0x13787a20, 0xea690bec, 0x07d34771, 0x200afc48, 0xffa2d732, 0xef4ef2e2, 0xf7cfd288,
    0x15d56d20, 0x20c05f13, 0x1b2604f7, 0x2a082b91, 0xf4c935d6, 0x05efad6c, 0x05eaf1bc, 0xfd2db7fc, 0xf26e9f00, 0x08dc79c5,
    0xf0909be5, 0x18abe97e, 0x1e13c538, 0xfb5266de, 0x07b1f059, 0xef81f822, 0xef5c9ba0, 0x204c0736, 0x07f0f4d9, 0x11354ca9,
    0xff4393b6, 0xffa0b5d3, 0xe6420adf, 0xfda29512, 0xf9e6c712, 0x14819176, 0xfb1eaf40, 0x1b72bdfe, 0x12f85bde, 0xf29fc5c3,
    0x055bc5ad, 0xde95cfd6, 0xf422392f, 0x1f219940, 0x091c01c1, 0x16ee7cf2,
};

q31_t result[MAT_SIZE];

int main(void)
{
    csi_matrix_instance_q31 src0;
    csi_matrix_instance_q31 src1;
    csi_matrix_instance_q31 dst;
    int i;

    csi_mat_init_q31(&src0, MAT_ROW, MAT_COL, data_src0);
    csi_mat_init_q31(&src1, MAT_ROW, MAT_COL, data_src1);
    csi_mat_init_q31(&dst, MAT_ROW, MAT_COL, result);

    csi_mat_mult_q31(&src0, &src1, &dst);

    for (i = 0; i < MAT_SIZE; i++) {
        if (result[i] != ref_result[i]) {
            wm_log_error("Example run failed!");
            while (1)
                ;
        }
    }

    wm_log_info("Example run successfully!");
    while (1)
        ;

    return 0;
}
