//
// c_corvus8_anim.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "g_local.h"

#pragma region ========================== Model definitions ==========================

#define FRAME_Breath1         	0
#define FRAME_Breath2         	1
#define FRAME_Breath3         	2
#define FRAME_Breath4         	3
#define FRAME_Breath5         	4
#define FRAME_Breath6         	5
#define FRAME_Breath7         	6
#define FRAME_Breath8         	7
#define FRAME_Breath9         	8
#define FRAME_Breath10        	9
#define FRAME_Breath11        	10
#define FRAME_Breath12        	11
#define FRAME_Breath13        	12
#define FRAME_Breath14        	13
#define FRAME_Breath15        	14
#define FRAME_Breath16        	15
#define FRAME_Breath17        	16
#define FRAME_Breath18        	17
#define FRAME_Breath19        	18
#define FRAME_Breath20        	19
#define FRAME_Breath21        	20
#define FRAME_Breath22        	21
#define FRAME_Breath23        	22
#define FRAME_butits1         	23
#define FRAME_butits2         	24
#define FRAME_butits3         	25
#define FRAME_butits4         	26
#define FRAME_butits5         	27
#define FRAME_butits6         	28
#define FRAME_butits7         	29
#define FRAME_butits8         	30
#define FRAME_butits9         	31
#define FRAME_butits10        	32
#define FRAME_butits11        	33
#define FRAME_butits12        	34
#define FRAME_butits13        	35
#define FRAME_butits14        	36
#define FRAME_butits15        	37
#define FRAME_butits16        	38
#define FRAME_butits17        	39
#define FRAME_butits18        	40
#define FRAME_butits19        	41
#define FRAME_butits20        	42
#define FRAME_butits21        	43
#define FRAME_butits22        	44
#define FRAME_butits23        	45
#define FRAME_butits24        	46
#define FRAME_bytome1         	47
#define FRAME_bytome2         	48
#define FRAME_bytome3         	49
#define FRAME_bytome4         	50
#define FRAME_bytome5         	51
#define FRAME_bytome6         	52
#define FRAME_bytome7         	53
#define FRAME_bytome8         	54
#define FRAME_bytome9         	55
#define FRAME_bytome10        	56
#define FRAME_bytome11        	57
#define FRAME_bytome12        	58
#define FRAME_bytome13        	59
#define FRAME_bytome14        	60
#define FRAME_bytome15        	61
#define FRAME_bytome16        	62
#define FRAME_bytome17        	63
#define FRAME_bytome18        	64
#define FRAME_cinewalk1       	65
#define FRAME_cinewalk2       	66
#define FRAME_cinewalk3       	67
#define FRAME_cinewalk4       	68
#define FRAME_cinewalk5       	69
#define FRAME_cinewalk6       	70
#define FRAME_cinewalk7       	71
#define FRAME_cinewalk8       	72
#define FRAME_cinewalk9       	73
#define FRAME_cinewalk10      	74
#define FRAME_cinewalk11      	75
#define FRAME_cinewalk12      	76
#define FRAME_cinewalk13      	77
#define FRAME_cinewalk14      	78
#define FRAME_cinewalk15      	79
#define FRAME_cinewalk16      	80
#define FRAME_cluster         	81
#define FRAME_gorun1          	82
#define FRAME_gorun2          	83
#define FRAME_gorun3          	84
#define FRAME_iam1            	85
#define FRAME_iam2            	86
#define FRAME_iam3            	87
#define FRAME_iam4            	88
#define FRAME_iam5            	89
#define FRAME_iam6            	90
#define FRAME_iam7            	91
#define FRAME_iam8            	92
#define FRAME_iam9            	93
#define FRAME_iam10           	94
#define FRAME_iam11           	95
#define FRAME_Ido1            	96
#define FRAME_Ido2            	97
#define FRAME_Ido3            	98
#define FRAME_Ido4            	99
#define FRAME_Ido5            	100
#define FRAME_Ido6            	101
#define FRAME_jog1            	102
#define FRAME_jog2            	103
#define FRAME_jog3            	104
#define FRAME_jog4            	105
#define FRAME_jog5            	106
#define FRAME_jog6            	107
#define FRAME_jog7            	108
#define FRAME_jog8            	109
#define FRAME_kneel1          	110
#define FRAME_kneel2          	111
#define FRAME_kneel3          	112
#define FRAME_kneel4          	113
#define FRAME_kneel5          	114
#define FRAME_kneel6          	115
#define FRAME_kneel7          	116
#define FRAME_kneel8          	117
#define FRAME_knlcyc1         	118
#define FRAME_knlcyc2         	119
#define FRAME_knlcyc3         	120
#define FRAME_knlcyc4         	121
#define FRAME_knlcyc5         	122
#define FRAME_knlcyc6         	123
#define FRAME_knlcyc7         	124
#define FRAME_knlcyc8         	125
#define FRAME_knlcyc9         	126
#define FRAME_knlcyc10        	127
#define FRAME_knlcyc11        	128
#define FRAME_knlcyc12        	129
#define FRAME_knltrn1         	130
#define FRAME_knltrn2         	131
#define FRAME_knltrn3         	132
#define FRAME_knltrn4         	133
#define FRAME_knltrn5         	134
#define FRAME_knltrn6         	135
#define FRAME_knltrn7         	136
#define FRAME_knltrn8         	137
#define FRAME_knltrn9         	138
#define FRAME_knltrn10        	139
#define FRAME_knltrn11        	140
#define FRAME_knltrn12        	141
#define FRAME_knltrn13        	142
#define FRAME_Lpivot1         	143
#define FRAME_Lpivot2         	144
#define FRAME_Lpivot3         	145
#define FRAME_Lpivot4         	146
#define FRAME_noharm1         	147
#define FRAME_noharm2         	148
#define FRAME_noharm3         	149
#define FRAME_noharm4         	150
#define FRAME_noharm5         	151
#define FRAME_noharm6         	152
#define FRAME_noharm7         	153
#define FRAME_noharm8         	154
#define FRAME_noharm9         	155
#define FRAME_noharm10        	156
#define FRAME_noharm11        	157
#define FRAME_noharm12        	158
#define FRAME_noharm13        	159
#define FRAME_noharm14        	160
#define FRAME_noharm15        	161
#define FRAME_noharm16        	162
#define FRAME_noharm17        	163
#define FRAME_noharm18        	164
#define FRAME_noharm19        	165
#define FRAME_noharm20        	166
#define FRAME_noharm21        	167
#define FRAME_noharm22        	168
#define FRAME_noharm23        	169
#define FRAME_noharm24        	170
#define FRAME_noharm25        	171
#define FRAME_noharm26        	172
#define FRAME_noharm27        	173
#define FRAME_noharm28        	174
#define FRAME_noharm29        	175
#define FRAME_noharm30        	176
#define FRAME_noharm31        	177
#define FRAME_stand1          	178
#define FRAME_stand2          	179
#define FRAME_stand3          	180
#define FRAME_stand4          	181
#define FRAME_stand5          	182
#define FRAME_stand6          	183
#define FRAME_stand7          	184
#define FRAME_stand8          	185
#define FRAME_willu1          	186
#define FRAME_willu2          	187
#define FRAME_willu3          	188
#define FRAME_willu4          	189
#define FRAME_willu5          	190
#define FRAME_willu6          	191
#define FRAME_willu7          	192
#define FRAME_willu8          	193
#define FRAME_willu9          	194
#define FRAME_willu10         	195
#define FRAME_arethe1         	196
#define FRAME_arethe2         	197
#define FRAME_arethe3         	198
#define FRAME_arethe4         	199
#define FRAME_arethe5         	200
#define FRAME_arethe6         	201
#define FRAME_arethe7         	202
#define FRAME_arethe8         	203
#define FRAME_arethe9         	204
#define FRAME_arethe10        	205
#define FRAME_arethe11        	206
#define FRAME_arethe12        	207
#define FRAME_arethe13        	208
#define FRAME_arethe14        	209
#define FRAME_arethe15        	210
#define FRAME_arethe16        	211
#define FRAME_arethe17        	212
#define FRAME_arethe18        	213
#define FRAME_arethe19        	214
#define FRAME_arethe20        	215
#define FRAME_arethe21        	216
#define FRAME_canit1          	217
#define FRAME_canit2          	218
#define FRAME_canit3          	219
#define FRAME_canit4          	220
#define FRAME_canit5          	221
#define FRAME_canit6          	222
#define FRAME_canit7          	223
#define FRAME_canit8          	224
#define FRAME_canit9          	225
#define FRAME_canit10         	226
#define FRAME_canit11         	227
#define FRAME_canit12         	228
#define FRAME_canit13         	229
#define FRAME_canit14         	230
#define FRAME_canit15         	231
#define FRAME_canit16         	232
#define FRAME_canit17         	233
#define FRAME_canit18         	234
#define FRAME_canit19         	235
#define FRAME_canit20         	236
#define FRAME_canit21         	237
#define FRAME_grtful1         	238
#define FRAME_grtful2         	239
#define FRAME_grtful3         	240
#define FRAME_grtful4         	241
#define FRAME_grtful5         	242
#define FRAME_grtful6         	243
#define FRAME_grtful7         	244
#define FRAME_grtful8         	245
#define FRAME_grtful9         	246
#define FRAME_grtful10        	247
#define FRAME_grtful11        	248
#define FRAME_grtful12        	249
#define FRAME_grtful13        	250
#define FRAME_grtful14        	251
#define FRAME_grtful15        	252
#define FRAME_grtful16        	253
#define FRAME_grtful17        	254
#define FRAME_grtful18        	255
#define FRAME_grtful19        	256
#define FRAME_grtful20        	257
#define FRAME_grtful21        	258
#define FRAME_grtful22        	259
#define FRAME_grtful23        	260
#define FRAME_grtful24        	261
#define FRAME_grtful25        	262
#define FRAME_grtful26        	263
#define FRAME_grtful27        	264
#define FRAME_grtful28        	265
#define FRAME_grtful29        	266
#define FRAME_grtful30        	267
#define FRAME_grtful31        	268
#define FRAME_grtful32        	269
#define FRAME_grtful33        	270
#define FRAME_grtful34        	271
#define FRAME_grtful35        	272
#define FRAME_grtful36        	273
#define FRAME_grtful37        	274
#define FRAME_grtful38        	275
#define FRAME_grtful39        	276
#define FRAME_grtful40        	277
#define FRAME_grtful41        	278
#define FRAME_grtful42        	279
#define FRAME_grtful43        	280
#define FRAME_grtful44        	281
#define FRAME_grtful45        	282
#define FRAME_grtful46        	283
#define FRAME_grtful47        	284
#define FRAME_grtful48        	285
#define FRAME_grtful49        	286
#define FRAME_grtful50        	287
#define FRAME_grtful51        	288
#define FRAME_grtful52        	289
#define FRAME_grtful53        	290
#define FRAME_grtful54        	291
#define FRAME_grtful55        	292
#define FRAME_grtful56        	293
#define FRAME_grtful57        	294
#define FRAME_grtful58        	295
#define FRAME_kelmnt1         	296
#define FRAME_kelmnt2         	297
#define FRAME_kelmnt3         	298
#define FRAME_kelmnt4         	299
#define FRAME_kelmnt5         	300
#define FRAME_kelmnt6         	301
#define FRAME_kelmnt7         	302
#define FRAME_kelmnt8         	303
#define FRAME_kelmnt9         	304
#define FRAME_kelmnt10        	305
#define FRAME_kelmnt11        	306
#define FRAME_kelmnt12        	307
#define FRAME_kelmnt13        	308
#define FRAME_kelmnt14        	309
#define FRAME_kelmnt15        	310
#define FRAME_kelmnt16        	311
#define FRAME_kelmnt17        	312
#define FRAME_kelmnt18        	313
#define FRAME_kelmnt19        	314
#define FRAME_kelmnt20        	315
#define FRAME_kelmnt21        	316
#define FRAME_kelmnt22        	317
#define FRAME_kelmnt23        	318
#define FRAME_kelmnt24        	319
#define FRAME_kelmnt25        	320
#define FRAME_kelmnt26        	321
#define FRAME_kelmnt27        	322
#define FRAME_kelmnt28        	323
#define FRAME_kelmnt29        	324
#define FRAME_kelmnt30        	325
#define FRAME_kelmnt31        	326
#define FRAME_kelmnt32        	327
#define FRAME_kelmnt33        	328
#define FRAME_kelmnt34        	329
#define FRAME_kelmnt35        	330
#define FRAME_kelmnt36        	331
#define FRAME_kelmnt37        	332
#define FRAME_kelmnt38        	333
#define FRAME_kelmnt39        	334
#define FRAME_kelmnt40        	335
#define FRAME_kelmnt41        	336
#define FRAME_kelmnt42        	337
#define FRAME_kelmnt43        	338
#define FRAME_kelmnt44        	339
#define FRAME_kelmnt45        	340
#define FRAME_kelmnt46        	341
#define FRAME_kelmnt47        	342
#define FRAME_kelmnt48        	343
#define FRAME_kelmnt49        	344
#define FRAME_kelmnt50        	345
#define FRAME_kelmnt51        	346
#define FRAME_kelmnt52        	347
#define FRAME_kelmnt53        	348
#define FRAME_kelmnt54        	349
#define FRAME_kelmnt55        	350
#define FRAME_kelmnt56        	351
#define FRAME_kelmnt57        	352
#define FRAME_kelmnt58        	353
#define FRAME_kelmnt59        	354
#define FRAME_kelmnt60        	355
#define FRAME_kelmnt61        	356
#define FRAME_kelmnt62        	357
#define FRAME_kelmnt63        	358
#define FRAME_kelmnt64        	359
#define FRAME_kelmnt65        	360
#define FRAME_kelmnt66        	361
#define FRAME_kelmnt67        	362
#define FRAME_kelmnt68        	363
#define FRAME_kelmnt69        	364
#define FRAME_kelmnt70        	365
#define FRAME_kelmnt71        	366
#define FRAME_kelmnt72        	367
#define FRAME_kelmnt73        	368
#define FRAME_kelmnt74        	369
#define FRAME_kelmnt75        	370
#define FRAME_kelmnt76        	371
#define FRAME_kelmnt77        	372
#define FRAME_potion1         	373
#define FRAME_potion2         	374
#define FRAME_potion3         	375
#define FRAME_potion4         	376
#define FRAME_potion5         	377
#define FRAME_potion6         	378
#define FRAME_potion7         	379
#define FRAME_potion8         	380
#define FRAME_potion9         	381
#define FRAME_potion10        	382
#define FRAME_potion11        	383
#define FRAME_potion12        	384
#define FRAME_potion13        	385
#define FRAME_potion14        	386
#define FRAME_potion15        	387
#define FRAME_potion16        	388
#define FRAME_potion17        	389
#define FRAME_potion18        	390
#define FRAME_potion19        	391
#define FRAME_potion20        	392
#define FRAME_potion21        	393
#define FRAME_potion22        	394
#define FRAME_potion23        	395
#define FRAME_potion24        	396
#define FRAME_potion25        	397
#define FRAME_potion26        	398
#define FRAME_potion27        	399
#define FRAME_potion28        	400
#define FRAME_potion29        	401
#define FRAME_potion30        	402
#define FRAME_potion31        	403
#define FRAME_potion32        	404
#define FRAME_potion33        	405
#define FRAME_potion34        	406
#define FRAME_potion35        	407
#define FRAME_potion36        	408
#define FRAME_potion37        	409
#define FRAME_potion38        	410
#define FRAME_potion39        	411
#define FRAME_potion40        	412
#define FRAME_potion41        	413
#define FRAME_potion42        	414
#define FRAME_potion43        	415
#define FRAME_potion44        	416
#define FRAME_potion45        	417
#define FRAME_potion46        	418
#define FRAME_potion47        	419
#define FRAME_potion48        	420
#define FRAME_potion49        	421
#define FRAME_potion50        	422
#define FRAME_potion51        	423
#define FRAME_pwalk1          	424
#define FRAME_pwalk2          	425
#define FRAME_pwalk3          	426
#define FRAME_pwalk4          	427
#define FRAME_pwalk5          	428
#define FRAME_pwalk6          	429
#define FRAME_pwalk7          	430
#define FRAME_pwalk8          	431
#define FRAME_pwalk9          	432
#define FRAME_pwalk10         	433
#define FRAME_pwalk11         	434
#define FRAME_pwalk12         	435
#define FRAME_pwalk13         	436
#define FRAME_pwalk14         	437
#define FRAME_pwalk15         	438
#define FRAME_pwalk16         	439
#define FRAME_pstop1          	440
#define FRAME_pstop2          	441
#define FRAME_pstop3          	442
#define FRAME_pstop4          	443
#define FRAME_pstop5          	444
#define FRAME_drink1          	445
#define FRAME_drink2          	446
#define FRAME_drink3          	447
#define FRAME_drink4          	448
#define FRAME_drink5          	449
#define FRAME_drink6          	450
#define FRAME_drink7          	451
#define FRAME_drink8          	452
#define FRAME_drink9          	453
#define FRAME_drink10         	454
#define FRAME_drink11         	455
#define FRAME_drink12         	456
#define FRAME_drink13         	457
#define FRAME_drink14         	458
#define FRAME_drink15         	459
#define FRAME_drink16         	460
#define FRAME_drink17         	461
#define FRAME_drink18         	462
#define FRAME_drink19         	463
#define FRAME_drink20         	464
#define FRAME_drink21         	465
#define FRAME_pour1           	466
#define FRAME_pour2           	467
#define FRAME_pour3           	468
#define FRAME_pour4           	469
#define FRAME_pour5           	470
#define FRAME_pour6           	471
#define FRAME_pour7           	472
#define FRAME_pour8           	473
#define FRAME_pour9           	474
#define FRAME_pour10          	475
#define FRAME_pour11          	476
#define FRAME_pour12          	477
#define FRAME_pour13          	478
#define FRAME_pour14          	479
#define FRAME_pour15          	480
#define FRAME_pour16          	481
#define FRAME_pour17          	482
#define FRAME_pour18          	483
#define FRAME_pour19          	484

#define MODEL_SCALE			1.0f

#define NUM_MESH_NODES		16

#define MESH_BASE2			0
#define MESH__BACK			1
#define MESH__STOFF			2
#define MESH__BOFF			3
#define MESH__ARMOR			4
#define MESH__RARM			5
#define MESH__RHANDHI		6
#define MESH__STAFACTV		7
#define MESH__BLADSTF		8
#define MESH__HELSTF		9
#define MESH__LARM			10
#define MESH__LHANDHI		11
#define MESH__BOWACTV		12
#define MESH__RLEG			13
#define MESH__LLEG			14
#define MESH__HEAD			15

#pragma endregion

typedef enum AnimID_e
{
	ANIM_C_ACTION1,
	ANIM_C_ACTION2,
	ANIM_C_ACTION3,
	ANIM_C_ACTION4,
	ANIM_C_ACTION5,
	ANIM_C_ACTION6,
	ANIM_C_ACTION7,
	ANIM_C_ACTION8,
	ANIM_C_ACTION9,
	ANIM_C_ACTION10,
	ANIM_C_ACTION11,
	ANIM_C_ACTION12,
	ANIM_C_ACTION13,
	ANIM_C_ACTION14,
	ANIM_C_ACTION15,
	ANIM_C_ACTION16,
	ANIM_C_ACTION17,
	ANIM_C_ACTION18,
	ANIM_C_ACTION19,
	ANIM_C_ACTION20,
	ANIM_C_IDLE1,
	ANIM_C_IDLE2,
	ANIM_C_WALKSTART,
	ANIM_C_WALK1,
	ANIM_C_WALK2,
	ANIM_C_WALK3,
	ANIM_C_WALKSTOP1,
	ANIM_C_WALKSTOP2,
	ANIM_C_PIVOTLEFTGO,
	ANIM_C_PIVOTLEFT,
	ANIM_C_PIVOTLEFTSTOP,
	ANIM_C_PIVOTRIGHTGO,
	ANIM_C_PIVOTRIGHT,
	ANIM_C_PIVOTRIGHTSTOP,
	NUM_ANIMS
} AnimID_t;

extern const animmove_t corvus8_move_c_action1;
extern const animmove_t corvus8_move_c_action2;
extern const animmove_t corvus8_move_c_action3;
extern const animmove_t corvus8_move_c_action4;
extern const animmove_t corvus8_move_c_action5;
extern const animmove_t corvus8_move_c_action6;
extern const animmove_t corvus8_move_c_action7;
extern const animmove_t corvus8_move_c_action8;
extern const animmove_t corvus8_move_c_action9;
extern const animmove_t corvus8_move_c_action10;
extern const animmove_t corvus8_move_c_action11;
extern const animmove_t corvus8_move_c_action12;
extern const animmove_t corvus8_move_c_action13;
extern const animmove_t corvus8_move_c_action14;
extern const animmove_t corvus8_move_c_action15;
extern const animmove_t corvus8_move_c_action16;
extern const animmove_t corvus8_move_c_action17;
extern const animmove_t corvus8_move_c_action18;
extern const animmove_t corvus8_move_c_action19;
extern const animmove_t corvus8_move_c_action20;
extern const animmove_t corvus8_move_c_idle1;
extern const animmove_t corvus8_move_c_idle2;
extern const animmove_t corvus8_move_c_walkstart;
extern const animmove_t corvus8_move_c_walk1;
extern const animmove_t corvus8_move_c_walk2;
extern const animmove_t corvus8_move_c_walk3;
extern const animmove_t corvus8_move_c_walkstop1;
extern const animmove_t corvus8_move_c_walkstop2;
extern const animmove_t corvus8_move_c_pivotleftgo;
extern const animmove_t corvus8_move_c_pivotleft;
extern const animmove_t corvus8_move_c_pivotleftstop;
extern const animmove_t corvus8_move_c_pivotrightgo;
extern const animmove_t corvus8_move_c_pivotright;
extern const animmove_t corvus8_move_c_pivotrightstop;
