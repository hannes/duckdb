/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     FCONST = 259,
     SCONST = 260,
     BCONST = 261,
     XCONST = 262,
     Op = 263,
     ICONST = 264,
     PARAM = 265,
     TYPECAST = 266,
     DOT_DOT = 267,
     COLON_EQUALS = 268,
     EQUALS_GREATER = 269,
     INTEGER_DIVISION = 270,
     POWER_OF = 271,
     LAMBDA_ARROW = 272,
     DOUBLE_ARROW = 273,
     LESS_EQUALS = 274,
     GREATER_EQUALS = 275,
     NOT_EQUALS = 276,
     ABORT_P = 277,
     ABSOLUTE_P = 278,
     ACCESS = 279,
     ACTION = 280,
     ADD_P = 281,
     ADMIN = 282,
     AFTER = 283,
     AGGREGATE = 284,
     ALL = 285,
     ALSO = 286,
     ALTER = 287,
     ALWAYS = 288,
     ANALYSE = 289,
     ANALYZE = 290,
     AND = 291,
     ANTI = 292,
     ANY = 293,
     ARRAY = 294,
     AS = 295,
     ASC_P = 296,
     ASOF = 297,
     ASSERTION = 298,
     ASSIGNMENT = 299,
     ASYMMETRIC = 300,
     AT = 301,
     ATTACH = 302,
     ATTRIBUTE = 303,
     AUTHORIZATION = 304,
     BACKWARD = 305,
     BEFORE = 306,
     BEGIN_P = 307,
     BETWEEN = 308,
     BIGINT = 309,
     BINARY = 310,
     BIT = 311,
     BOOLEAN_P = 312,
     BOTH = 313,
     BY = 314,
     CACHE = 315,
     CALL_P = 316,
     CALLED = 317,
     CASCADE = 318,
     CASCADED = 319,
     CASE = 320,
     CAST = 321,
     CATALOG_P = 322,
     CENTURIES_P = 323,
     CENTURY_P = 324,
     CHAIN = 325,
     CHAR_P = 326,
     CHARACTER = 327,
     CHARACTERISTICS = 328,
     CHECK_P = 329,
     CHECKPOINT = 330,
     CLASS = 331,
     CLOSE = 332,
     CLUSTER = 333,
     COALESCE = 334,
     COLLATE = 335,
     COLLATION = 336,
     COLUMN = 337,
     COLUMNS = 338,
     COMMENT = 339,
     COMMENTS = 340,
     COMMIT = 341,
     COMMITTED = 342,
     COMPRESSION = 343,
     CONCURRENTLY = 344,
     CONFIGURATION = 345,
     CONFLICT = 346,
     CONNECTION = 347,
     CONSTRAINT = 348,
     CONSTRAINTS = 349,
     CONTENT_P = 350,
     CONTINUE_P = 351,
     CONVERSION_P = 352,
     COPY = 353,
     COST = 354,
     CREATE_P = 355,
     CROSS = 356,
     CSV = 357,
     CUBE = 358,
     CURRENT_P = 359,
     CURSOR = 360,
     CYCLE = 361,
     DATA_P = 362,
     DATABASE = 363,
     DAY_P = 364,
     DAYS_P = 365,
     DEALLOCATE = 366,
     DEC = 367,
     DECADE_P = 368,
     DECADES_P = 369,
     DECIMAL_P = 370,
     DECLARE = 371,
     DEFAULT = 372,
     DEFAULTS = 373,
     DEFERRABLE = 374,
     DEFERRED = 375,
     DEFINE = 376,
     DEFINER = 377,
     DELETE_P = 378,
     DELIMITER = 379,
     DELIMITERS = 380,
     DEPENDS = 381,
     DESC_P = 382,
     DESCRIBE = 383,
     DETACH = 384,
     DICTIONARY = 385,
     DISABLE_P = 386,
     DISCARD = 387,
     DISTINCT = 388,
     DO = 389,
     DOCUMENT_P = 390,
     DOMAIN_P = 391,
     DOUBLE_P = 392,
     DROP = 393,
     EACH = 394,
     ELSE = 395,
     ENABLE_P = 396,
     ENCODING = 397,
     ENCRYPTED = 398,
     END_P = 399,
     ENUM_P = 400,
     ESCAPE = 401,
     EVENT = 402,
     EXCEPT = 403,
     EXCLUDE = 404,
     EXCLUDING = 405,
     EXCLUSIVE = 406,
     EXECUTE = 407,
     EXISTS = 408,
     EXPLAIN = 409,
     EXPORT_P = 410,
     EXPORT_STATE = 411,
     EXTENSION = 412,
     EXTENSIONS = 413,
     EXTERNAL = 414,
     EXTRACT = 415,
     FALSE_P = 416,
     FAMILY = 417,
     FETCH = 418,
     FILTER = 419,
     FIRST_P = 420,
     FLOAT_P = 421,
     FOLLOWING = 422,
     FOR = 423,
     FORCE = 424,
     FOREIGN = 425,
     FORWARD = 426,
     FREEZE = 427,
     FROM = 428,
     FULL = 429,
     FUNCTION = 430,
     FUNCTIONS = 431,
     GENERATED = 432,
     GLOB = 433,
     GLOBAL = 434,
     GRANT = 435,
     GRANTED = 436,
     GROUP_P = 437,
     GROUPING = 438,
     GROUPING_ID = 439,
     GROUPS = 440,
     HANDLER = 441,
     HAVING = 442,
     HEADER_P = 443,
     HOLD = 444,
     HOUR_P = 445,
     HOURS_P = 446,
     IDENTITY_P = 447,
     IF_P = 448,
     IGNORE_P = 449,
     ILIKE = 450,
     IMMEDIATE = 451,
     IMMUTABLE = 452,
     IMPLICIT_P = 453,
     IMPORT_P = 454,
     IN_P = 455,
     INCLUDE_P = 456,
     INCLUDING = 457,
     INCREMENT = 458,
     INDEX = 459,
     INDEXES = 460,
     INHERIT = 461,
     INHERITS = 462,
     INITIALLY = 463,
     INLINE_P = 464,
     INNER_P = 465,
     INOUT = 466,
     INPUT_P = 467,
     INSENSITIVE = 468,
     INSERT = 469,
     INSTALL = 470,
     INSTEAD = 471,
     INT_P = 472,
     INTEGER = 473,
     INTERSECT = 474,
     INTERVAL = 475,
     INTO = 476,
     INVOKER = 477,
     IS = 478,
     ISNULL = 479,
     ISOLATION = 480,
     JOIN = 481,
     JSON = 482,
     KEY = 483,
     LABEL = 484,
     LANGUAGE = 485,
     LARGE_P = 486,
     LAST_P = 487,
     LATERAL_P = 488,
     LEADING = 489,
     LEAKPROOF = 490,
     LEFT = 491,
     LEVEL = 492,
     LIKE = 493,
     LIMIT = 494,
     LISTEN = 495,
     LOAD = 496,
     LOCAL = 497,
     LOCATION = 498,
     LOCK_P = 499,
     LOCKED = 500,
     LOGGED = 501,
     MACRO = 502,
     MAP = 503,
     MAPPING = 504,
     MATCH = 505,
     MATCH_RECOGNIZE = 506,
     MATERIALIZED = 507,
     MAXVALUE = 508,
     MEASURES = 509,
     METHOD = 510,
     MICROSECOND_P = 511,
     MICROSECONDS_P = 512,
     MILLENNIA_P = 513,
     MILLENNIUM_P = 514,
     MILLISECOND_P = 515,
     MILLISECONDS_P = 516,
     MINUTE_P = 517,
     MINUTES_P = 518,
     MINVALUE = 519,
     MODE = 520,
     MONTH_P = 521,
     MONTHS_P = 522,
     MOVE = 523,
     NAME_P = 524,
     NAMES = 525,
     NATIONAL = 526,
     NATURAL = 527,
     NCHAR = 528,
     NEW = 529,
     NEXT = 530,
     NO = 531,
     NONE = 532,
     NOT = 533,
     NOTHING = 534,
     NOTIFY = 535,
     NOTNULL = 536,
     NOWAIT = 537,
     NULL_P = 538,
     NULLIF = 539,
     NULLS_P = 540,
     NUMERIC = 541,
     OBJECT_P = 542,
     OF = 543,
     OFF = 544,
     OFFSET = 545,
     OIDS = 546,
     OLD = 547,
     ON = 548,
     ONE = 549,
     ONLY = 550,
     OPERATOR = 551,
     OPTION = 552,
     OPTIONS = 553,
     OR = 554,
     ORDER = 555,
     ORDINALITY = 556,
     OTHERS = 557,
     OUT_P = 558,
     OUTER_P = 559,
     OVER = 560,
     OVERLAPS = 561,
     OVERLAY = 562,
     OVERRIDING = 563,
     OWNED = 564,
     OWNER = 565,
     PARALLEL = 566,
     PARSER = 567,
     PARTIAL = 568,
     PARTITION = 569,
     PARTITIONED = 570,
     PASSING = 571,
     PASSWORD = 572,
     PAST = 573,
     PATTERN = 574,
     PER = 575,
     PERCENT = 576,
     PERSISTENT = 577,
     PIVOT = 578,
     PIVOT_LONGER = 579,
     PIVOT_WIDER = 580,
     PLACING = 581,
     PLANS = 582,
     POLICY = 583,
     POSITION = 584,
     POSITIONAL = 585,
     PRAGMA_P = 586,
     PRECEDING = 587,
     PRECISION = 588,
     PREPARE = 589,
     PREPARED = 590,
     PRESERVE = 591,
     PRIMARY = 592,
     PRIOR = 593,
     PRIVILEGES = 594,
     PROCEDURAL = 595,
     PROCEDURE = 596,
     PROGRAM = 597,
     PUBLICATION = 598,
     QUALIFY = 599,
     QUARTER_P = 600,
     QUARTERS_P = 601,
     QUOTE = 602,
     RANGE = 603,
     READ_P = 604,
     REAL = 605,
     REASSIGN = 606,
     RECHECK = 607,
     RECURSIVE = 608,
     REF = 609,
     REFERENCES = 610,
     REFERENCING = 611,
     REFRESH = 612,
     REINDEX = 613,
     RELATIVE_P = 614,
     RELEASE = 615,
     RENAME = 616,
     REPEATABLE = 617,
     REPLACE = 618,
     REPLICA = 619,
     RESET = 620,
     RESPECT_P = 621,
     RESTART = 622,
     RESTRICT = 623,
     RETURNING = 624,
     RETURNS = 625,
     REVOKE = 626,
     RIGHT = 627,
     ROLE = 628,
     ROLLBACK = 629,
     ROLLUP = 630,
     ROW = 631,
     ROWS = 632,
     RULE = 633,
     SAMPLE = 634,
     SAVEPOINT = 635,
     SCHEMA = 636,
     SCHEMAS = 637,
     SCOPE = 638,
     SCROLL = 639,
     SEARCH = 640,
     SECOND_P = 641,
     SECONDS_P = 642,
     SECRET = 643,
     SECURITY = 644,
     SELECT = 645,
     SEMI = 646,
     SEQUENCE = 647,
     SEQUENCES = 648,
     SERIALIZABLE = 649,
     SERVER = 650,
     SESSION = 651,
     SET = 652,
     SETOF = 653,
     SETS = 654,
     SHARE = 655,
     SHOW = 656,
     SIMILAR = 657,
     SIMPLE = 658,
     SKIP = 659,
     SMALLINT = 660,
     SNAPSHOT = 661,
     SOME = 662,
     SORTED = 663,
     SQL_P = 664,
     STABLE = 665,
     STANDALONE_P = 666,
     START = 667,
     STATEMENT = 668,
     STATISTICS = 669,
     STDIN = 670,
     STDOUT = 671,
     STORAGE = 672,
     STORED = 673,
     STRICT_P = 674,
     STRIP_P = 675,
     STRUCT = 676,
     SUBSCRIPTION = 677,
     SUBSTRING = 678,
     SUMMARIZE = 679,
     SYMMETRIC = 680,
     SYSID = 681,
     SYSTEM_P = 682,
     TABLE = 683,
     TABLES = 684,
     TABLESAMPLE = 685,
     TABLESPACE = 686,
     TEMP = 687,
     TEMPLATE = 688,
     TEMPORARY = 689,
     TEXT_P = 690,
     THEN = 691,
     TIES = 692,
     TIME = 693,
     TIMESTAMP = 694,
     TO = 695,
     TRAILING = 696,
     TRANSACTION = 697,
     TRANSFORM = 698,
     TREAT = 699,
     TRIGGER = 700,
     TRIM = 701,
     TRUE_P = 702,
     TRUNCATE = 703,
     TRUSTED = 704,
     TRY_CAST = 705,
     TYPE_P = 706,
     TYPES_P = 707,
     UNBOUNDED = 708,
     UNCOMMITTED = 709,
     UNENCRYPTED = 710,
     UNION = 711,
     UNIQUE = 712,
     UNKNOWN = 713,
     UNLISTEN = 714,
     UNLOGGED = 715,
     UNPACK = 716,
     UNPIVOT = 717,
     UNTIL = 718,
     UPDATE = 719,
     USE_P = 720,
     USER = 721,
     USING = 722,
     VACUUM = 723,
     VALID = 724,
     VALIDATE = 725,
     VALIDATOR = 726,
     VALUE_P = 727,
     VALUES = 728,
     VARCHAR = 729,
     VARIABLE_P = 730,
     VARIADIC = 731,
     VARYING = 732,
     VERBOSE = 733,
     VERSION_P = 734,
     VIEW = 735,
     VIEWS = 736,
     VIRTUAL = 737,
     VOLATILE = 738,
     WEEK_P = 739,
     WEEKS_P = 740,
     WHEN = 741,
     WHERE = 742,
     WHITESPACE_P = 743,
     WINDOW = 744,
     WITH = 745,
     WITHIN = 746,
     WITHOUT = 747,
     WORK = 748,
     WRAPPER = 749,
     WRITE_P = 750,
     XML_P = 751,
     XMLATTRIBUTES = 752,
     XMLCONCAT = 753,
     XMLELEMENT = 754,
     XMLEXISTS = 755,
     XMLFOREST = 756,
     XMLNAMESPACES = 757,
     XMLPARSE = 758,
     XMLPI = 759,
     XMLROOT = 760,
     XMLSERIALIZE = 761,
     XMLTABLE = 762,
     YEAR_P = 763,
     YEARS_P = 764,
     YES_P = 765,
     ZONE = 766,
     NOT_LA = 767,
     NULLS_LA = 768,
     WITH_LA = 769,
     POSTFIXOP = 770,
     UMINUS = 771
   };
#endif
/* Tokens.  */
#define IDENT 258
#define FCONST 259
#define SCONST 260
#define BCONST 261
#define XCONST 262
#define Op 263
#define ICONST 264
#define PARAM 265
#define TYPECAST 266
#define DOT_DOT 267
#define COLON_EQUALS 268
#define EQUALS_GREATER 269
#define INTEGER_DIVISION 270
#define POWER_OF 271
#define LAMBDA_ARROW 272
#define DOUBLE_ARROW 273
#define LESS_EQUALS 274
#define GREATER_EQUALS 275
#define NOT_EQUALS 276
#define ABORT_P 277
#define ABSOLUTE_P 278
#define ACCESS 279
#define ACTION 280
#define ADD_P 281
#define ADMIN 282
#define AFTER 283
#define AGGREGATE 284
#define ALL 285
#define ALSO 286
#define ALTER 287
#define ALWAYS 288
#define ANALYSE 289
#define ANALYZE 290
#define AND 291
#define ANTI 292
#define ANY 293
#define ARRAY 294
#define AS 295
#define ASC_P 296
#define ASOF 297
#define ASSERTION 298
#define ASSIGNMENT 299
#define ASYMMETRIC 300
#define AT 301
#define ATTACH 302
#define ATTRIBUTE 303
#define AUTHORIZATION 304
#define BACKWARD 305
#define BEFORE 306
#define BEGIN_P 307
#define BETWEEN 308
#define BIGINT 309
#define BINARY 310
#define BIT 311
#define BOOLEAN_P 312
#define BOTH 313
#define BY 314
#define CACHE 315
#define CALL_P 316
#define CALLED 317
#define CASCADE 318
#define CASCADED 319
#define CASE 320
#define CAST 321
#define CATALOG_P 322
#define CENTURIES_P 323
#define CENTURY_P 324
#define CHAIN 325
#define CHAR_P 326
#define CHARACTER 327
#define CHARACTERISTICS 328
#define CHECK_P 329
#define CHECKPOINT 330
#define CLASS 331
#define CLOSE 332
#define CLUSTER 333
#define COALESCE 334
#define COLLATE 335
#define COLLATION 336
#define COLUMN 337
#define COLUMNS 338
#define COMMENT 339
#define COMMENTS 340
#define COMMIT 341
#define COMMITTED 342
#define COMPRESSION 343
#define CONCURRENTLY 344
#define CONFIGURATION 345
#define CONFLICT 346
#define CONNECTION 347
#define CONSTRAINT 348
#define CONSTRAINTS 349
#define CONTENT_P 350
#define CONTINUE_P 351
#define CONVERSION_P 352
#define COPY 353
#define COST 354
#define CREATE_P 355
#define CROSS 356
#define CSV 357
#define CUBE 358
#define CURRENT_P 359
#define CURSOR 360
#define CYCLE 361
#define DATA_P 362
#define DATABASE 363
#define DAY_P 364
#define DAYS_P 365
#define DEALLOCATE 366
#define DEC 367
#define DECADE_P 368
#define DECADES_P 369
#define DECIMAL_P 370
#define DECLARE 371
#define DEFAULT 372
#define DEFAULTS 373
#define DEFERRABLE 374
#define DEFERRED 375
#define DEFINE 376
#define DEFINER 377
#define DELETE_P 378
#define DELIMITER 379
#define DELIMITERS 380
#define DEPENDS 381
#define DESC_P 382
#define DESCRIBE 383
#define DETACH 384
#define DICTIONARY 385
#define DISABLE_P 386
#define DISCARD 387
#define DISTINCT 388
#define DO 389
#define DOCUMENT_P 390
#define DOMAIN_P 391
#define DOUBLE_P 392
#define DROP 393
#define EACH 394
#define ELSE 395
#define ENABLE_P 396
#define ENCODING 397
#define ENCRYPTED 398
#define END_P 399
#define ENUM_P 400
#define ESCAPE 401
#define EVENT 402
#define EXCEPT 403
#define EXCLUDE 404
#define EXCLUDING 405
#define EXCLUSIVE 406
#define EXECUTE 407
#define EXISTS 408
#define EXPLAIN 409
#define EXPORT_P 410
#define EXPORT_STATE 411
#define EXTENSION 412
#define EXTENSIONS 413
#define EXTERNAL 414
#define EXTRACT 415
#define FALSE_P 416
#define FAMILY 417
#define FETCH 418
#define FILTER 419
#define FIRST_P 420
#define FLOAT_P 421
#define FOLLOWING 422
#define FOR 423
#define FORCE 424
#define FOREIGN 425
#define FORWARD 426
#define FREEZE 427
#define FROM 428
#define FULL 429
#define FUNCTION 430
#define FUNCTIONS 431
#define GENERATED 432
#define GLOB 433
#define GLOBAL 434
#define GRANT 435
#define GRANTED 436
#define GROUP_P 437
#define GROUPING 438
#define GROUPING_ID 439
#define GROUPS 440
#define HANDLER 441
#define HAVING 442
#define HEADER_P 443
#define HOLD 444
#define HOUR_P 445
#define HOURS_P 446
#define IDENTITY_P 447
#define IF_P 448
#define IGNORE_P 449
#define ILIKE 450
#define IMMEDIATE 451
#define IMMUTABLE 452
#define IMPLICIT_P 453
#define IMPORT_P 454
#define IN_P 455
#define INCLUDE_P 456
#define INCLUDING 457
#define INCREMENT 458
#define INDEX 459
#define INDEXES 460
#define INHERIT 461
#define INHERITS 462
#define INITIALLY 463
#define INLINE_P 464
#define INNER_P 465
#define INOUT 466
#define INPUT_P 467
#define INSENSITIVE 468
#define INSERT 469
#define INSTALL 470
#define INSTEAD 471
#define INT_P 472
#define INTEGER 473
#define INTERSECT 474
#define INTERVAL 475
#define INTO 476
#define INVOKER 477
#define IS 478
#define ISNULL 479
#define ISOLATION 480
#define JOIN 481
#define JSON 482
#define KEY 483
#define LABEL 484
#define LANGUAGE 485
#define LARGE_P 486
#define LAST_P 487
#define LATERAL_P 488
#define LEADING 489
#define LEAKPROOF 490
#define LEFT 491
#define LEVEL 492
#define LIKE 493
#define LIMIT 494
#define LISTEN 495
#define LOAD 496
#define LOCAL 497
#define LOCATION 498
#define LOCK_P 499
#define LOCKED 500
#define LOGGED 501
#define MACRO 502
#define MAP 503
#define MAPPING 504
#define MATCH 505
#define MATCH_RECOGNIZE 506
#define MATERIALIZED 507
#define MAXVALUE 508
#define MEASURES 509
#define METHOD 510
#define MICROSECOND_P 511
#define MICROSECONDS_P 512
#define MILLENNIA_P 513
#define MILLENNIUM_P 514
#define MILLISECOND_P 515
#define MILLISECONDS_P 516
#define MINUTE_P 517
#define MINUTES_P 518
#define MINVALUE 519
#define MODE 520
#define MONTH_P 521
#define MONTHS_P 522
#define MOVE 523
#define NAME_P 524
#define NAMES 525
#define NATIONAL 526
#define NATURAL 527
#define NCHAR 528
#define NEW 529
#define NEXT 530
#define NO 531
#define NONE 532
#define NOT 533
#define NOTHING 534
#define NOTIFY 535
#define NOTNULL 536
#define NOWAIT 537
#define NULL_P 538
#define NULLIF 539
#define NULLS_P 540
#define NUMERIC 541
#define OBJECT_P 542
#define OF 543
#define OFF 544
#define OFFSET 545
#define OIDS 546
#define OLD 547
#define ON 548
#define ONE 549
#define ONLY 550
#define OPERATOR 551
#define OPTION 552
#define OPTIONS 553
#define OR 554
#define ORDER 555
#define ORDINALITY 556
#define OTHERS 557
#define OUT_P 558
#define OUTER_P 559
#define OVER 560
#define OVERLAPS 561
#define OVERLAY 562
#define OVERRIDING 563
#define OWNED 564
#define OWNER 565
#define PARALLEL 566
#define PARSER 567
#define PARTIAL 568
#define PARTITION 569
#define PARTITIONED 570
#define PASSING 571
#define PASSWORD 572
#define PAST 573
#define PATTERN 574
#define PER 575
#define PERCENT 576
#define PERSISTENT 577
#define PIVOT 578
#define PIVOT_LONGER 579
#define PIVOT_WIDER 580
#define PLACING 581
#define PLANS 582
#define POLICY 583
#define POSITION 584
#define POSITIONAL 585
#define PRAGMA_P 586
#define PRECEDING 587
#define PRECISION 588
#define PREPARE 589
#define PREPARED 590
#define PRESERVE 591
#define PRIMARY 592
#define PRIOR 593
#define PRIVILEGES 594
#define PROCEDURAL 595
#define PROCEDURE 596
#define PROGRAM 597
#define PUBLICATION 598
#define QUALIFY 599
#define QUARTER_P 600
#define QUARTERS_P 601
#define QUOTE 602
#define RANGE 603
#define READ_P 604
#define REAL 605
#define REASSIGN 606
#define RECHECK 607
#define RECURSIVE 608
#define REF 609
#define REFERENCES 610
#define REFERENCING 611
#define REFRESH 612
#define REINDEX 613
#define RELATIVE_P 614
#define RELEASE 615
#define RENAME 616
#define REPEATABLE 617
#define REPLACE 618
#define REPLICA 619
#define RESET 620
#define RESPECT_P 621
#define RESTART 622
#define RESTRICT 623
#define RETURNING 624
#define RETURNS 625
#define REVOKE 626
#define RIGHT 627
#define ROLE 628
#define ROLLBACK 629
#define ROLLUP 630
#define ROW 631
#define ROWS 632
#define RULE 633
#define SAMPLE 634
#define SAVEPOINT 635
#define SCHEMA 636
#define SCHEMAS 637
#define SCOPE 638
#define SCROLL 639
#define SEARCH 640
#define SECOND_P 641
#define SECONDS_P 642
#define SECRET 643
#define SECURITY 644
#define SELECT 645
#define SEMI 646
#define SEQUENCE 647
#define SEQUENCES 648
#define SERIALIZABLE 649
#define SERVER 650
#define SESSION 651
#define SET 652
#define SETOF 653
#define SETS 654
#define SHARE 655
#define SHOW 656
#define SIMILAR 657
#define SIMPLE 658
#define SKIP 659
#define SMALLINT 660
#define SNAPSHOT 661
#define SOME 662
#define SORTED 663
#define SQL_P 664
#define STABLE 665
#define STANDALONE_P 666
#define START 667
#define STATEMENT 668
#define STATISTICS 669
#define STDIN 670
#define STDOUT 671
#define STORAGE 672
#define STORED 673
#define STRICT_P 674
#define STRIP_P 675
#define STRUCT 676
#define SUBSCRIPTION 677
#define SUBSTRING 678
#define SUMMARIZE 679
#define SYMMETRIC 680
#define SYSID 681
#define SYSTEM_P 682
#define TABLE 683
#define TABLES 684
#define TABLESAMPLE 685
#define TABLESPACE 686
#define TEMP 687
#define TEMPLATE 688
#define TEMPORARY 689
#define TEXT_P 690
#define THEN 691
#define TIES 692
#define TIME 693
#define TIMESTAMP 694
#define TO 695
#define TRAILING 696
#define TRANSACTION 697
#define TRANSFORM 698
#define TREAT 699
#define TRIGGER 700
#define TRIM 701
#define TRUE_P 702
#define TRUNCATE 703
#define TRUSTED 704
#define TRY_CAST 705
#define TYPE_P 706
#define TYPES_P 707
#define UNBOUNDED 708
#define UNCOMMITTED 709
#define UNENCRYPTED 710
#define UNION 711
#define UNIQUE 712
#define UNKNOWN 713
#define UNLISTEN 714
#define UNLOGGED 715
#define UNPACK 716
#define UNPIVOT 717
#define UNTIL 718
#define UPDATE 719
#define USE_P 720
#define USER 721
#define USING 722
#define VACUUM 723
#define VALID 724
#define VALIDATE 725
#define VALIDATOR 726
#define VALUE_P 727
#define VALUES 728
#define VARCHAR 729
#define VARIABLE_P 730
#define VARIADIC 731
#define VARYING 732
#define VERBOSE 733
#define VERSION_P 734
#define VIEW 735
#define VIEWS 736
#define VIRTUAL 737
#define VOLATILE 738
#define WEEK_P 739
#define WEEKS_P 740
#define WHEN 741
#define WHERE 742
#define WHITESPACE_P 743
#define WINDOW 744
#define WITH 745
#define WITHIN 746
#define WITHOUT 747
#define WORK 748
#define WRAPPER 749
#define WRITE_P 750
#define XML_P 751
#define XMLATTRIBUTES 752
#define XMLCONCAT 753
#define XMLELEMENT 754
#define XMLEXISTS 755
#define XMLFOREST 756
#define XMLNAMESPACES 757
#define XMLPARSE 758
#define XMLPI 759
#define XMLROOT 760
#define XMLSERIALIZE 761
#define XMLTABLE 762
#define YEAR_P 763
#define YEARS_P 764
#define YES_P 765
#define ZONE 766
#define NOT_LA 767
#define NULLS_LA 768
#define WITH_LA 769
#define POSTFIXOP 770
#define UMINUS 771




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 14 "third_party/libpg_query/grammar/grammar.y"
{
	core_YYSTYPE		core_yystype;
	/* these fields must match core_YYSTYPE: */
	int					ival;
	char				*str;
	const char			*keyword;
	const char          *conststr;

	char				chr;
	bool				boolean;
	PGJoinType			jtype;
	PGDropBehavior		dbehavior;
	PGOnCommitAction		oncommit;
	PGOnCreateConflict		oncreateconflict;
	PGList				*list;
	PGNode				*node;
	PGValue				*value;
	PGObjectType			objtype;
	PGTypeName			*typnam;
	PGObjectWithArgs		*objwithargs;
	PGDefElem				*defelt;
	PGSortBy				*sortby;
	PGWindowDef			*windef;
	PGJoinExpr			*jexpr;
	PGIndexElem			*ielem;
	PGAlias				*alias;
	PGRangeVar			*range;
	PGIntoClause			*into;
	PGCTEMaterialize			ctematerialize;
	PGMatchRecognizeRowsPerMatch			rows_per_match;
	PGMatchRecognizeAfterMatchClause			*after_match_clause;
	PGWithClause			*with;
	PGInferClause			*infer;
	PGOnConflictClause	*onconflict;
	PGOnConflictActionAlias onconflictshorthand;
	PGAIndices			*aind;
	PGResTarget			*target;
	PGInsertStmt			*istmt;
	PGVariableSetStmt		*vsetstmt;
	PGOverridingKind       override;
	PGSortByDir            sortorder;
	PGSortByNulls          nullorder;
	PGIgnoreNulls          ignorenulls;
	PGConstrType           constr;
	PGLockClauseStrength lockstrength;
	PGLockWaitPolicy lockwaitpolicy;
	PGSubLinkType subquerytype;
	PGViewCheckOption viewcheckoption;
	PGInsertColumnOrder bynameorposition;
	PGLoadInstallType loadinstalltype;
	PGTransactionStmtType transactiontype;
}
/* Line 1529 of yacc.c.  */
#line 1134 "third_party/libpg_query/grammar/grammar_out.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


