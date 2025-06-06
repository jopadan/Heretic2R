//
// c_siernan1_anim.h
//
// Copyright 1998 Raven Software
//

#pragma once

#include "g_local.h"

#pragma region ========================== Model definitions ==========================

#define FRAME_A1              	0
#define FRAME_A2              	1
#define FRAME_A3              	2
#define FRAME_A4              	3
#define FRAME_A5              	4
#define FRAME_A6              	5
#define FRAME_A7              	6
#define FRAME_A8              	7
#define FRAME_A9              	8
#define FRAME_A10             	9
#define FRAME_A11             	10
#define FRAME_A12             	11
#define FRAME_A13             	12
#define FRAME_A14             	13
#define FRAME_A15             	14
#define FRAME_A16             	15
#define FRAME_A17             	16
#define FRAME_A18             	17
#define FRAME_A19             	18
#define FRAME_A20             	19
#define FRAME_A21             	20
#define FRAME_B1              	21
#define FRAME_B2              	22
#define FRAME_B3              	23
#define FRAME_B4              	24
#define FRAME_B5              	25
#define FRAME_B6              	26
#define FRAME_B7              	27
#define FRAME_B8              	28
#define FRAME_B9              	29
#define FRAME_B10             	30
#define FRAME_B11             	31
#define FRAME_B12             	32
#define FRAME_B13             	33
#define FRAME_B14             	34
#define FRAME_B15             	35
#define FRAME_B16             	36
#define FRAME_B17             	37
#define FRAME_B18             	38
#define FRAME_B19             	39
#define FRAME_B20             	40
#define FRAME_C1              	41
#define FRAME_C2              	42
#define FRAME_C3              	43
#define FRAME_C4              	44
#define FRAME_C5              	45
#define FRAME_C6              	46
#define FRAME_C7              	47
#define FRAME_C8              	48
#define FRAME_C9              	49
#define FRAME_C10             	50
#define FRAME_C11             	51
#define FRAME_C12             	52
#define FRAME_C13             	53
#define FRAME_C14             	54
#define FRAME_C15             	55
#define FRAME_C16             	56
#define FRAME_D1              	57
#define FRAME_D2              	58
#define FRAME_D3              	59
#define FRAME_D4              	60
#define FRAME_D5              	61
#define FRAME_D6              	62
#define FRAME_D7              	63
#define FRAME_D8              	64
#define FRAME_D9              	65
#define FRAME_D10             	66
#define FRAME_D11             	67
#define FRAME_E1              	68
#define FRAME_E2              	69
#define FRAME_E3              	70
#define FRAME_E4              	71
#define FRAME_E5              	72
#define FRAME_E6              	73
#define FRAME_E7              	74
#define FRAME_E8              	75
#define FRAME_E9              	76
#define FRAME_E10             	77
#define FRAME_E11             	78
#define FRAME_E12             	79
#define FRAME_E13             	80
#define FRAME_E14             	81
#define FRAME_E15             	82
#define FRAME_E16             	83
#define FRAME_E17             	84
#define FRAME_E18             	85
#define FRAME_E19             	86
#define FRAME_E20             	87
#define FRAME_E21             	88
#define FRAME_E22             	89
#define FRAME_E23             	90
#define FRAME_E24             	91
#define FRAME_E25             	92
#define FRAME_E26             	93
#define FRAME_E27             	94
#define FRAME_E28             	95
#define FRAME_E29             	96
#define FRAME_E30             	97
#define FRAME_E31             	98
#define FRAME_E32             	99
#define FRAME_E33             	100
#define FRAME_E34             	101
#define FRAME_E35             	102
#define FRAME_E36             	103
#define FRAME_E37             	104
#define FRAME_Idle1           	105
#define FRAME_Idle2           	106
#define FRAME_Idle3           	107
#define FRAME_Idle4           	108
#define FRAME_Idle5           	109
#define FRAME_Idle6           	110
#define FRAME_Idle7           	111
#define FRAME_Idle8           	112
#define FRAME_Idle9           	113
#define FRAME_Idle10          	114
#define FRAME_Idle11          	115
#define FRAME_Idle12          	116
#define FRAME_Idle13          	117
#define FRAME_Idle14          	118
#define FRAME_Idle15          	119
#define FRAME_Idle16          	120
#define FRAME_Idle17          	121
#define FRAME_Idle18          	122
#define FRAME_Idle19          	123
#define FRAME_Idle20          	124
#define FRAME_Idle21          	125
#define FRAME_Idle22          	126
#define FRAME_Idle23          	127
#define FRAME_Idle24          	128
#define FRAME_poly            	129
#define FRAME_Siernan         	130
#define FRAME_stpwalk1        	131
#define FRAME_stpwalk2        	132
#define FRAME_stpwalk3        	133
#define FRAME_stpwalk4        	134
#define FRAME_stpwalk5        	135
#define FRAME_strtwalk1       	136
#define FRAME_strtwalk2       	137
#define FRAME_strtwalk3       	138
#define FRAME_strtwalk4       	139
#define FRAME_strtwalk5       	140
#define FRAME_strtwalk6       	141
#define FRAME_walk1           	142
#define FRAME_walk2           	143
#define FRAME_walk3           	144
#define FRAME_walk4           	145
#define FRAME_walk5           	146
#define FRAME_walk6           	147
#define FRAME_walk7           	148
#define FRAME_walk8           	149
#define FRAME_walk9           	150
#define FRAME_walk10          	151
#define FRAME_walk11          	152
#define FRAME_walk12          	153
#define FRAME_uhave1          	154
#define FRAME_uhave2          	155
#define FRAME_uhave3          	156
#define FRAME_uhave4          	157
#define FRAME_uhave5          	158
#define FRAME_uhave6          	159
#define FRAME_uhave7          	160
#define FRAME_uhave8          	161
#define FRAME_uhave9          	162
#define FRAME_uhave10         	163
#define FRAME_uhave11         	164
#define FRAME_uhave12         	165
#define FRAME_uhave13         	166
#define FRAME_uhave14         	167
#define FRAME_uhave15         	168
#define FRAME_uhave16         	169
#define FRAME_uhave17         	170
#define FRAME_uhave18         	171
#define FRAME_uhave19         	172
#define FRAME_uhave20         	173
#define FRAME_uhave21         	174
#define FRAME_uhave22         	175
#define FRAME_uhave23         	176
#define FRAME_uhave24         	177
#define FRAME_uhave25         	178
#define FRAME_uhave26         	179
#define FRAME_uhave27         	180
#define FRAME_uhave28         	181
#define FRAME_uhave29         	182
#define FRAME_uhave30         	183
#define FRAME_uhave31         	184
#define FRAME_uhave32         	185
#define FRAME_uhave33         	186
#define FRAME_uhave34         	187
#define FRAME_uhave35         	188
#define FRAME_uhave36         	189
#define FRAME_uhave37         	190
#define FRAME_uhave38         	191
#define FRAME_uhave39         	192
#define FRAME_uhave40         	193
#define FRAME_uhave41         	194
#define FRAME_uhave42         	195
#define FRAME_uhave43         	196
#define FRAME_uhave44         	197
#define FRAME_uhave45         	198
#define FRAME_uhave46         	199
#define FRAME_uhave47         	200
#define FRAME_uhave48         	201
#define FRAME_uhave49         	202
#define FRAME_uhave50         	203
#define FRAME_uhave51         	204
#define FRAME_uhave52         	205
#define FRAME_uhave53         	206
#define FRAME_uhave54         	207
#define FRAME_uhave55         	208
#define FRAME_uhave56         	209
#define FRAME_uhave57         	210
#define FRAME_uhave58         	211
#define FRAME_uhave59         	212
#define FRAME_uhave60         	213
#define FRAME_uhave61         	214
#define FRAME_uhave62         	215
#define FRAME_uhave63         	216
#define FRAME_uhave64         	217
#define FRAME_uhave65         	218
#define FRAME_uhave66         	219
#define FRAME_uhave67         	220
#define FRAME_uhave68         	221
#define FRAME_uhave69         	222
#define FRAME_uhave70         	223
#define FRAME_uhave71         	224
#define FRAME_uhave72         	225
#define FRAME_uhave73         	226
#define FRAME_uhave74         	227
#define FRAME_uhave75         	228
#define FRAME_uhave76         	229
#define FRAME_uhave77         	230
#define FRAME_uhave78         	231
#define FRAME_uhave79         	232
#define FRAME_uhave80         	233
#define FRAME_uhave81         	234
#define FRAME_uhave82         	235
#define FRAME_uhave83         	236
#define FRAME_uhave84         	237
#define FRAME_uhave85         	238
#define FRAME_uhave86         	239
#define FRAME_uhave87         	240
#define FRAME_uhave88         	241
#define FRAME_uhave89         	242
#define FRAME_uhave90         	243
#define FRAME_uhave91         	244
#define FRAME_uhave92         	245
#define FRAME_uhave93         	246
#define FRAME_uhave94         	247
#define FRAME_uhave95         	248
#define FRAME_uhave96         	249
#define FRAME_uhave97         	250
#define FRAME_uhave98         	251
#define FRAME_uhave99         	252
#define FRAME_uhave100        	253
#define FRAME_uhave101        	254
#define FRAME_uhave102        	255
#define FRAME_uhave103        	256
#define FRAME_uhave104        	257
#define FRAME_uhave105        	258
#define FRAME_uhave106        	259
#define FRAME_uhave107        	260
#define FRAME_uhave108        	261
#define FRAME_uhave109        	262
#define FRAME_uhave110        	263
#define FRAME_uhave111        	264
#define FRAME_uhave112        	265
#define FRAME_uhave113        	266
#define FRAME_uhave114        	267
#define FRAME_uhave115        	268
#define FRAME_uhave116        	269
#define FRAME_uhave117        	270
#define FRAME_uhave118        	271
#define FRAME_uhave119        	272
#define FRAME_uhave120        	273
#define FRAME_uhave121        	274
#define FRAME_uhave122        	275
#define FRAME_uhave123        	276
#define FRAME_uhave124        	277
#define FRAME_uhave125        	278
#define FRAME_uhave126        	279
#define FRAME_uhave127        	280
#define FRAME_uhave128        	281
#define FRAME_uhave129        	282
#define FRAME_uhave130        	283
#define FRAME_uhave131        	284
#define FRAME_uhave132        	285
#define FRAME_uhave133        	286
#define FRAME_uhave134        	287
#define FRAME_uhave135        	288
#define FRAME_uhave136        	289
#define FRAME_rember1         	290
#define FRAME_rember2         	291
#define FRAME_rember3         	292
#define FRAME_rember4         	293
#define FRAME_rember5         	294
#define FRAME_rember6         	295
#define FRAME_rember7         	296
#define FRAME_rember8         	297
#define FRAME_rember9         	298
#define FRAME_rember10        	299
#define FRAME_rember11        	300
#define FRAME_rember12        	301
#define FRAME_rember13        	302
#define FRAME_rember14        	303
#define FRAME_rember15        	304
#define FRAME_rember16        	305
#define FRAME_rember17        	306
#define FRAME_rember18        	307
#define FRAME_rember19        	308
#define FRAME_rember20        	309
#define FRAME_rember21        	310
#define FRAME_rember22        	311
#define FRAME_rember23        	312
#define FRAME_rember24        	313
#define FRAME_rember25        	314
#define FRAME_rember26        	315
#define FRAME_rember27        	316
#define FRAME_rember28        	317
#define FRAME_rember29        	318
#define FRAME_rember30        	319
#define FRAME_rember31        	320
#define FRAME_rember32        	321
#define FRAME_rember33        	322
#define FRAME_rember34        	323
#define FRAME_rember35        	324
#define FRAME_rember36        	325
#define FRAME_rember37        	326
#define FRAME_rember38        	327
#define FRAME_rember39        	328
#define FRAME_rember40        	329
#define FRAME_rember41        	330
#define FRAME_rember42        	331
#define FRAME_rember43        	332
#define FRAME_rember44        	333
#define FRAME_rember45        	334
#define FRAME_rember46        	335
#define FRAME_rember47        	336
#define FRAME_rember48        	337
#define FRAME_rember49        	338
#define FRAME_rember50        	339
#define FRAME_rember51        	340
#define FRAME_rember52        	341
#define FRAME_rember53        	342
#define FRAME_rember54        	343
#define FRAME_rember55        	344
#define FRAME_rember56        	345
#define FRAME_rember57        	346
#define FRAME_rember58        	347
#define FRAME_rember59        	348
#define FRAME_rember60        	349
#define FRAME_rember61        	350
#define FRAME_rember62        	351
#define FRAME_rember63        	352
#define FRAME_rember64        	353
#define FRAME_rember65        	354
#define FRAME_rember66        	355
#define FRAME_rember67        	356
#define FRAME_rember68        	357
#define FRAME_noknow1         	358
#define FRAME_noknow2         	359
#define FRAME_noknow3         	360
#define FRAME_noknow4         	361
#define FRAME_noknow5         	362
#define FRAME_noknow6         	363
#define FRAME_noknow7         	364
#define FRAME_noknow8         	365
#define FRAME_noknow9         	366
#define FRAME_noknow10        	367
#define FRAME_noknow11        	368
#define FRAME_noknow12        	369
#define FRAME_noknow13        	370
#define FRAME_noknow14        	371
#define FRAME_noknow15        	372
#define FRAME_noknow16        	373
#define FRAME_noknow17        	374
#define FRAME_noknow18        	375
#define FRAME_noknow19        	376
#define FRAME_noknow20        	377
#define FRAME_noknow21        	378
#define FRAME_noknow22        	379
#define FRAME_noknow23        	380
#define FRAME_noknow24        	381
#define FRAME_noknow25        	382
#define FRAME_noknow26        	383
#define FRAME_noknow27        	384
#define FRAME_noknow28        	385
#define FRAME_noknow29        	386
#define FRAME_noknow30        	387
#define FRAME_noknow31        	388
#define FRAME_noknow32        	389
#define FRAME_noknow33        	390
#define FRAME_noknow34        	391
#define FRAME_noknow35        	392
#define FRAME_noknow36        	393
#define FRAME_noknow37        	394
#define FRAME_noknow38        	395
#define FRAME_noknow39        	396
#define FRAME_noknow40        	397
#define FRAME_noknow41        	398
#define FRAME_noknow42        	399
#define FRAME_noknow43        	400
#define FRAME_noknow44        	401
#define FRAME_noknow45        	402
#define FRAME_noknow46        	403
#define FRAME_noknow47        	404
#define FRAME_noknow48        	405
#define FRAME_noknow49        	406
#define FRAME_noknow50        	407
#define FRAME_noknow51        	408
#define FRAME_noknow52        	409
#define FRAME_noknow53        	410
#define FRAME_noknow54        	411
#define FRAME_noknow55        	412
#define FRAME_noknow56        	413
#define FRAME_noknow57        	414
#define FRAME_noknow58        	415
#define FRAME_noknow59        	416
#define FRAME_noknow60        	417
#define FRAME_noknow61        	418
#define FRAME_noknow62        	419
#define FRAME_noknow63        	420
#define FRAME_noknow64        	421
#define FRAME_noknow65        	422
#define FRAME_noknow66        	423
#define FRAME_noknow67        	424
#define FRAME_noknow68        	425
#define FRAME_noknow69        	426
#define FRAME_noknow70        	427
#define FRAME_noknow71        	428
#define FRAME_noknow72        	429
#define FRAME_noknow73        	430
#define FRAME_noknow74        	431
#define FRAME_noknow75        	432
#define FRAME_noknow76        	433
#define FRAME_noknow77        	434
#define FRAME_noknow78        	435
#define FRAME_noknow79        	436
#define FRAME_noknow80        	437
#define FRAME_noknow81        	438
#define FRAME_ifear1          	439
#define FRAME_ifear2          	440
#define FRAME_ifear3          	441
#define FRAME_ifear4          	442
#define FRAME_ifear5          	443
#define FRAME_ifear6          	444
#define FRAME_ifear7          	445
#define FRAME_ifear8          	446
#define FRAME_ifear9          	447
#define FRAME_ifear10         	448
#define FRAME_ifear11         	449
#define FRAME_ifear12         	450
#define FRAME_ifear13         	451
#define FRAME_ifear14         	452
#define FRAME_ifear15         	453
#define FRAME_ifear16         	454
#define FRAME_ifear17         	455
#define FRAME_ifear18         	456
#define FRAME_ifear19         	457
#define FRAME_ifear20         	458
#define FRAME_ifear21         	459
#define FRAME_ifear22         	460
#define FRAME_ifear23         	461
#define FRAME_ifear24         	462
#define FRAME_ifear25         	463
#define FRAME_ifear26         	464
#define FRAME_ifear27         	465
#define FRAME_ifear28         	466
#define FRAME_ifear29         	467
#define FRAME_ifear30         	468
#define FRAME_ifear31         	469
#define FRAME_ifear32         	470
#define FRAME_ifear33         	471
#define FRAME_ifear34         	472
#define FRAME_ifear35         	473
#define FRAME_ifear36         	474
#define FRAME_ifear37         	475
#define FRAME_ifear38         	476
#define FRAME_ifear39         	477
#define FRAME_ifear40         	478
#define FRAME_ifear41         	479
#define FRAME_ifear42         	480
#define FRAME_ifear43         	481
#define FRAME_ifear44         	482
#define FRAME_ifear45         	483
#define FRAME_ifear46         	484
#define FRAME_ifear47         	485
#define FRAME_ifear48         	486
#define FRAME_ifear49         	487
#define FRAME_ifear50         	488
#define FRAME_ifear51         	489
#define FRAME_ifear52         	490
#define FRAME_ifear53         	491
#define FRAME_ifear54         	492
#define FRAME_ifear55         	493
#define FRAME_ifear56         	494
#define FRAME_ifear57         	495
#define FRAME_ifear58         	496
#define FRAME_ifear59         	497
#define FRAME_ifear60         	498
#define FRAME_ifear61         	499
#define FRAME_ifear62         	500
#define FRAME_ifear63         	501
#define FRAME_ifear64         	502
#define FRAME_ifear65         	503
#define FRAME_ifear66         	504
#define FRAME_ifear67         	505
#define FRAME_ifear68         	506
#define FRAME_ifear69         	507
#define FRAME_ifear70         	508
#define FRAME_ifear71         	509
#define FRAME_ifear72         	510
#define FRAME_ifear73         	511
#define FRAME_ifear74         	512
#define FRAME_ifear75         	513
#define FRAME_ifear76         	514
#define FRAME_ifear77         	515
#define FRAME_ifear78         	516
#define FRAME_ifear79         	517
#define FRAME_ifear80         	518
#define FRAME_ifear81         	519
#define FRAME_ifear82         	520
#define FRAME_ifear83         	521
#define FRAME_ifear84         	522
#define FRAME_ifear85         	523
#define FRAME_ifear86         	524
#define FRAME_ifear87         	525
#define FRAME_ifear88         	526
#define FRAME_ifear89         	527
#define FRAME_ifear90         	528
#define FRAME_ifear91         	529
#define FRAME_ifear92         	530
#define FRAME_ifear93         	531
#define FRAME_ifear94         	532
#define FRAME_ifear95         	533
#define FRAME_ifear96         	534
#define FRAME_ifear97         	535
#define FRAME_ifear98         	536
#define FRAME_ifear99         	537
#define FRAME_ifear100        	538
#define FRAME_ifear101        	539
#define FRAME_ifear102        	540
#define FRAME_ifear103        	541
#define FRAME_ifear104        	542
#define FRAME_ifear105        	543
#define FRAME_ifear106        	544
#define FRAME_ifear107        	545
#define FRAME_ifear108        	546
#define FRAME_ifear109        	547
#define FRAME_ifear110        	548
#define FRAME_ifear111        	549
#define FRAME_ifear112        	550
#define FRAME_ifear113        	551
#define FRAME_ifear114        	552
#define FRAME_ifear115        	553
#define FRAME_ifear116        	554
#define FRAME_ifear117        	555
#define FRAME_notin1          	556
#define FRAME_notin2          	557
#define FRAME_notin3          	558
#define FRAME_notin4          	559
#define FRAME_notin5          	560
#define FRAME_notin6          	561
#define FRAME_notin7          	562
#define FRAME_notin8          	563
#define FRAME_notin9          	564
#define FRAME_notin10         	565
#define FRAME_notin11         	566
#define FRAME_notin12         	567
#define FRAME_notin13         	568
#define FRAME_notin14         	569
#define FRAME_notin15         	570
#define FRAME_notin16         	571
#define FRAME_notin17         	572
#define FRAME_notin18         	573
#define FRAME_notin19         	574
#define FRAME_notin20         	575
#define FRAME_notin21         	576
#define FRAME_notin22         	577
#define FRAME_notin23         	578
#define FRAME_notin24         	579
#define FRAME_notin25         	580
#define FRAME_notin26         	581
#define FRAME_notin27         	582
#define FRAME_notin28         	583
#define FRAME_notin29         	584
#define FRAME_notin30         	585
#define FRAME_notin31         	586
#define FRAME_notin32         	587
#define FRAME_notin33         	588
#define FRAME_notin34         	589
#define FRAME_notin35         	590
#define FRAME_notin36         	591
#define FRAME_notin37         	592
#define FRAME_notin38         	593
#define FRAME_notin39         	594
#define FRAME_notin40         	595
#define FRAME_notin41         	596
#define FRAME_notin42         	597
#define FRAME_notin43         	598
#define FRAME_notin44         	599
#define FRAME_notin45         	600
#define FRAME_notin46         	601
#define FRAME_notin47         	602
#define FRAME_notin48         	603
#define FRAME_notin49         	604
#define FRAME_notin50         	605
#define FRAME_notin51         	606
#define FRAME_notin52         	607
#define FRAME_notin53         	608
#define FRAME_notin54         	609
#define FRAME_notin55         	610
#define FRAME_notin56         	611
#define FRAME_notin57         	612
#define FRAME_notin58         	613
#define FRAME_notin59         	614
#define FRAME_notin60         	615
#define FRAME_notin61         	616
#define FRAME_notin62         	617
#define FRAME_notin63         	618
#define FRAME_notin64         	619
#define FRAME_notin65         	620
#define FRAME_notin66         	621
#define FRAME_notin67         	622
#define FRAME_notin68         	623
#define FRAME_notin69         	624
#define FRAME_notin70         	625
#define FRAME_notin71         	626
#define FRAME_notin72         	627
#define FRAME_notin73         	628
#define FRAME_notin74         	629
#define FRAME_notin75         	630
#define FRAME_notin76         	631
#define FRAME_notin77         	632
#define FRAME_notin78         	633
#define FRAME_notin79         	634
#define FRAME_notin80         	635
#define FRAME_notin81         	636
#define FRAME_notin82         	637
#define FRAME_notin83         	638
#define FRAME_notin84         	639
#define FRAME_notin85         	640
#define FRAME_notin86         	641
#define FRAME_notin87         	642
#define FRAME_notin88         	643
#define FRAME_notin89         	644
#define FRAME_notin90         	645
#define FRAME_notin91         	646
#define FRAME_notin92         	647
#define FRAME_notin93         	648
#define FRAME_notin94         	649
#define FRAME_notin95         	650
#define FRAME_notin96         	651
#define FRAME_notin97         	652
#define FRAME_notin98         	653
#define FRAME_notin99         	654
#define FRAME_notin100        	655
#define FRAME_notin101        	656
#define FRAME_notin102        	657
#define FRAME_notin103        	658
#define FRAME_notin104        	659
#define FRAME_notin105        	660
#define FRAME_notin106        	661
#define FRAME_notin107        	662
#define FRAME_notin108        	663
#define FRAME_notin109        	664
#define FRAME_notin110        	665
#define FRAME_notin111        	666
#define FRAME_notin112        	667
#define FRAME_notin113        	668
#define FRAME_notin114        	669
#define FRAME_notin115        	670
#define FRAME_notin116        	671
#define FRAME_notin117        	672
#define FRAME_notin118        	673
#define FRAME_notin119        	674
#define FRAME_notin120        	675
#define FRAME_notin121        	676
#define FRAME_notin122        	677
#define FRAME_notin123        	678
#define FRAME_notin124        	679
#define FRAME_notin125        	680
#define FRAME_notin126        	681
#define FRAME_notin127        	682
#define FRAME_notin128        	683
#define FRAME_notin129        	684
#define FRAME_notin130        	685
#define FRAME_notin131        	686
#define FRAME_notin132        	687
#define FRAME_notin133        	688
#define FRAME_notin134        	689
#define FRAME_notin135        	690
#define FRAME_notin136        	691
#define FRAME_notin137        	692
#define FRAME_notin138        	693
#define FRAME_notin139        	694
#define FRAME_notin140        	695
#define FRAME_notin141        	696
#define FRAME_notin142        	697
#define FRAME_notin143        	698
#define FRAME_yes1            	699
#define FRAME_yes2            	700
#define FRAME_yes3            	701
#define FRAME_yes4            	702
#define FRAME_yes5            	703
#define FRAME_yes6            	704
#define FRAME_yes7            	705
#define FRAME_yes8            	706
#define FRAME_yes9            	707
#define FRAME_yes10           	708
#define FRAME_yes11           	709
#define FRAME_yes12           	710
#define FRAME_yes13           	711
#define FRAME_yes14           	712
#define FRAME_yes15           	713
#define FRAME_yes16           	714
#define FRAME_yes17           	715
#define FRAME_yes18           	716
#define FRAME_yes19           	717
#define FRAME_yes20           	718
#define FRAME_yes21           	719
#define FRAME_yes22           	720
#define FRAME_yes23           	721
#define FRAME_yes24           	722
#define FRAME_yes25           	723
#define FRAME_yes26           	724
#define FRAME_yes27           	725
#define FRAME_yes28           	726
#define FRAME_yes29           	727
#define FRAME_yes30           	728
#define FRAME_yes31           	729
#define FRAME_yes32           	730
#define FRAME_yes33           	731
#define FRAME_yes34           	732
#define FRAME_yes35           	733
#define FRAME_yes36           	734
#define FRAME_yes37           	735
#define FRAME_yes38           	736
#define FRAME_yes39           	737
#define FRAME_yes40           	738
#define FRAME_yes41           	739
#define FRAME_yes42           	740
#define FRAME_yes43           	741
#define FRAME_yes44           	742
#define FRAME_yes45           	743
#define FRAME_yes46           	744
#define FRAME_yes47           	745
#define FRAME_yes48           	746
#define FRAME_yes49           	747
#define FRAME_yes50           	748
#define FRAME_yes51           	749
#define FRAME_yes52           	750
#define FRAME_yes53           	751
#define FRAME_yes54           	752
#define FRAME_yes55           	753
#define FRAME_yes56           	754
#define FRAME_yes57           	755
#define FRAME_yes58           	756
#define FRAME_yes59           	757
#define FRAME_yes60           	758
#define FRAME_yes61           	759
#define FRAME_yes62           	760
#define FRAME_yes63           	761
#define FRAME_yes64           	762
#define FRAME_yes65           	763
#define FRAME_yes66           	764
#define FRAME_yes67           	765
#define FRAME_yes68           	766
#define FRAME_yes69           	767
#define FRAME_yes70           	768
#define FRAME_yes71           	769
#define FRAME_yes72           	770
#define FRAME_yes73           	771
#define FRAME_yes74           	772
#define FRAME_yes75           	773
#define FRAME_yes76           	774
#define FRAME_yes77           	775
#define FRAME_yes78           	776
#define FRAME_yes79           	777
#define FRAME_yes80           	778
#define FRAME_yes81           	779
#define FRAME_yes82           	780
#define FRAME_yes83           	781
#define FRAME_yes84           	782
#define FRAME_yes85           	783
#define FRAME_yes86           	784
#define FRAME_yes87           	785
#define FRAME_yes88           	786
#define FRAME_yes89           	787
#define FRAME_yes90           	788
#define FRAME_yes91           	789
#define FRAME_yes92           	790
#define FRAME_yes93           	791
#define FRAME_yes94           	792
#define FRAME_yes95           	793
#define FRAME_yes96           	794
#define FRAME_yes97           	795
#define FRAME_yes98           	796
#define FRAME_yes99           	797
#define FRAME_yes100          	798
#define FRAME_siping1         	799
#define FRAME_siping2         	800
#define FRAME_siping3         	801
#define FRAME_siping4         	802
#define FRAME_siping5         	803
#define FRAME_siping6         	804
#define FRAME_siping7         	805
#define FRAME_siping8         	806
#define FRAME_siping9         	807
#define FRAME_siping10        	808
#define FRAME_siping11        	809
#define FRAME_siping12        	810
#define FRAME_siping13        	811
#define FRAME_siping14        	812
#define FRAME_siping15        	813
#define FRAME_siping16        	814
#define FRAME_siping17        	815
#define FRAME_siping18        	816
#define FRAME_siping19        	817
#define FRAME_siping20        	818
#define FRAME_siping21        	819
#define FRAME_siping22        	820
#define FRAME_accept1         	821
#define FRAME_accept2         	822
#define FRAME_accept3         	823
#define FRAME_accept4         	824
#define FRAME_accept5         	825
#define FRAME_accept6         	826
#define FRAME_accept7         	827
#define FRAME_accept8         	828
#define FRAME_accept9         	829

#define NUM_MESH_NODES		6

#define MESH_BASEBIN		0
#define MESH__TORSO			1
#define MESH__HEAD1			2
#define MESH__CENTERSPIKE1	3
#define MESH__LEFTARM		4
#define MESH__RIGHTARM		5

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
	ANIM_C_IDLE1,
	ANIM_C_IDLE2,
	ANIM_C_IDLE3,
	ANIM_C_IDLE4,
	ANIM_C_IDLE5,
	ANIM_C_WALKSTART,
	ANIM_C_WALK1,
	ANIM_C_WALKSTOP1,
	NUM_ANIMS
} AnimID_t;

extern const animmove_t siernan1_move_c_action1;
extern const animmove_t siernan1_move_c_action2;
extern const animmove_t siernan1_move_c_action3;
extern const animmove_t siernan1_move_c_action4;
extern const animmove_t siernan1_move_c_action5;
extern const animmove_t siernan1_move_c_action6;
extern const animmove_t siernan1_move_c_action7;
extern const animmove_t siernan1_move_c_action8;
extern const animmove_t siernan1_move_c_action9;
extern const animmove_t siernan1_move_c_action10;
extern const animmove_t siernan1_move_c_action11;
extern const animmove_t siernan1_move_c_action12;
extern const animmove_t siernan1_move_c_action13;
extern const animmove_t siernan1_move_c_idle1;
extern const animmove_t siernan1_move_c_idle2;
extern const animmove_t siernan1_move_c_idle3;
extern const animmove_t siernan1_move_c_idle4;
extern const animmove_t siernan1_move_c_idle5;
extern const animmove_t siernan1_move_c_walkstart;
extern const animmove_t siernan1_move_c_walk1;
extern const animmove_t siernan1_move_c_walkstop1;