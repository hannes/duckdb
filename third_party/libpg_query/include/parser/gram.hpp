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
     SINGLE_ARROW = 272,
     DOUBLE_ARROW = 273,
     SINGLE_COLON = 274,
     LESS_EQUALS = 275,
     GREATER_EQUALS = 276,
     NOT_EQUALS = 277,
     ABORT_P = 278,
     ABSOLUTE_P = 279,
     ACCESS = 280,
     ACTION = 281,
     ADD_P = 282,
     ADMIN = 283,
     AFTER = 284,
     AGGREGATE = 285,
     ALL = 286,
     ALSO = 287,
     ALTER = 288,
     ALWAYS = 289,
     ANALYSE = 290,
     ANALYZE = 291,
     AND = 292,
     ANTI = 293,
     ANY = 294,
     ARRAY = 295,
     AS = 296,
     ASC_P = 297,
     ASOF = 298,
     ASSERTION = 299,
     ASSIGNMENT = 300,
     ASYMMETRIC = 301,
     AT = 302,
     ATTACH = 303,
     ATTRIBUTE = 304,
     AUTHORIZATION = 305,
     BACKWARD = 306,
     BEFORE = 307,
     BEGIN_P = 308,
     BETWEEN = 309,
     BIGINT = 310,
     BINARY = 311,
     BIT = 312,
     BOOLEAN_P = 313,
     BOTH = 314,
     BY = 315,
     CACHE = 316,
     CALL_P = 317,
     CALLED = 318,
     CASCADE = 319,
     CASCADED = 320,
     CASE = 321,
     CAST = 322,
     CATALOG_P = 323,
     CENTURIES_P = 324,
     CENTURY_P = 325,
     CHAIN = 326,
     CHAR_P = 327,
     CHARACTER = 328,
     CHARACTERISTICS = 329,
     CHECK_P = 330,
     CHECKPOINT = 331,
     CLASS = 332,
     CLOSE = 333,
     CLUSTER = 334,
     COALESCE = 335,
     COLLATE = 336,
     COLLATION = 337,
     COLUMN = 338,
     COLUMNS = 339,
     COMMENT = 340,
     COMMENTS = 341,
     COMMIT = 342,
     COMMITTED = 343,
     COMPRESSION = 344,
     CONCURRENTLY = 345,
     CONFIGURATION = 346,
     CONFLICT = 347,
     CONNECTION = 348,
     CONSTRAINT = 349,
     CONSTRAINTS = 350,
     CONTENT_P = 351,
     CONTINUE_P = 352,
     CONVERSION_P = 353,
     COPY = 354,
     COST = 355,
     CREATE_P = 356,
     CROSS = 357,
     CSV = 358,
     CUBE = 359,
     CURRENT_P = 360,
     CURSOR = 361,
     CYCLE = 362,
     DATA_P = 363,
     DATABASE = 364,
     DAY_P = 365,
     DAYS_P = 366,
     DEALLOCATE = 367,
     DEC = 368,
     DECADE_P = 369,
     DECADES_P = 370,
     DECIMAL_P = 371,
     DECLARE = 372,
     DEFAULT = 373,
     DEFAULTS = 374,
     DEFERRABLE = 375,
     DEFERRED = 376,
     DEFINE = 377,
     DEFINER = 378,
     DELETE_P = 379,
     DELIMITER = 380,
     DELIMITERS = 381,
     DEPENDS = 382,
     DESC_P = 383,
     DESCRIBE = 384,
     DETACH = 385,
     DICTIONARY = 386,
     DISABLE_P = 387,
     DISCARD = 388,
     DISTINCT = 389,
     DO = 390,
     DOCUMENT_P = 391,
     DOMAIN_P = 392,
     DOUBLE_P = 393,
     DROP = 394,
     EACH = 395,
     ELSE = 396,
     ENABLE_P = 397,
     ENCODING = 398,
     ENCRYPTED = 399,
     END_P = 400,
     ENUM_P = 401,
     ERROR_P = 402,
     ESCAPE = 403,
     EVENT = 404,
     EXCEPT = 405,
     EXCLUDE = 406,
     EXCLUDING = 407,
     EXCLUSIVE = 408,
     EXECUTE = 409,
     EXISTS = 410,
     EXPLAIN = 411,
     EXPORT_P = 412,
     EXPORT_STATE = 413,
     EXTENSION = 414,
     EXTENSIONS = 415,
     EXTERNAL = 416,
     EXTRACT = 417,
     FALSE_P = 418,
     FAMILY = 419,
     FETCH = 420,
     FILTER = 421,
     FIRST_P = 422,
     FLOAT_P = 423,
     FOLLOWING = 424,
     FOR = 425,
     FORCE = 426,
     FOREIGN = 427,
     FORWARD = 428,
     FREEZE = 429,
     FROM = 430,
     FULL = 431,
     FUNCTION = 432,
     FUNCTIONS = 433,
     GENERATED = 434,
     GLOB = 435,
     GLOBAL = 436,
     GRANT = 437,
     GRANTED = 438,
     GROUP_P = 439,
     GROUPING = 440,
     GROUPING_ID = 441,
     GROUPS = 442,
     HANDLER = 443,
     HAVING = 444,
     HEADER_P = 445,
     HOLD = 446,
     HOUR_P = 447,
     HOURS_P = 448,
     IDENTITY_P = 449,
     IF_P = 450,
     IGNORE_P = 451,
     ILIKE = 452,
     IMMEDIATE = 453,
     IMMUTABLE = 454,
     IMPLICIT_P = 455,
     IMPORT_P = 456,
     IN_P = 457,
     INCLUDE_P = 458,
     INCLUDING = 459,
     INCREMENT = 460,
     INDEX = 461,
     INDEXES = 462,
     INHERIT = 463,
     INHERITS = 464,
     INITIALLY = 465,
     INLINE_P = 466,
     INNER_P = 467,
     INOUT = 468,
     INPUT_P = 469,
     INSENSITIVE = 470,
     INSERT = 471,
     INSTALL = 472,
     INSTEAD = 473,
     INT_P = 474,
     INTEGER = 475,
     INTERSECT = 476,
     INTERVAL = 477,
     INTO = 478,
     INVOKER = 479,
     IS = 480,
     ISNULL = 481,
     ISOLATION = 482,
     JOIN = 483,
     JSON = 484,
     KEY = 485,
     LABEL = 486,
     LAMBDA = 487,
     LANGUAGE = 488,
     LARGE_P = 489,
     LAST_P = 490,
     LATERAL_P = 491,
     LEADING = 492,
     LEAKPROOF = 493,
     LEFT = 494,
     LEVEL = 495,
     LIKE = 496,
     LIMIT = 497,
     LISTEN = 498,
     LOAD = 499,
     LOCAL = 500,
     LOCATION = 501,
     LOCK_P = 502,
     LOCKED = 503,
     LOGGED = 504,
     MACRO = 505,
     MAP = 506,
     MAPPING = 507,
     MATCH = 508,
     MATCH_RECOGNIZE = 509,
     MATCHED = 510,
     MATERIALIZED = 511,
     MAXVALUE = 512,
     MEASURES = 513,
     MERGE = 514,
     METHOD = 515,
     MICROSECOND_P = 516,
     MICROSECONDS_P = 517,
     MILLENNIA_P = 518,
     MILLENNIUM_P = 519,
     MILLISECOND_P = 520,
     MILLISECONDS_P = 521,
     MINUTE_P = 522,
     MINUTES_P = 523,
     MINVALUE = 524,
     MODE = 525,
     MONTH_P = 526,
     MONTHS_P = 527,
     MOVE = 528,
     NAME_P = 529,
     NAMES = 530,
     NATIONAL = 531,
     NATURAL = 532,
     NCHAR = 533,
     NEW = 534,
     NEXT = 535,
     NO = 536,
     NONE = 537,
     NOT = 538,
     NOTHING = 539,
     NOTIFY = 540,
     NOTNULL = 541,
     NOWAIT = 542,
     NULL_P = 543,
     NULLIF = 544,
     NULLS_P = 545,
     NUMERIC = 546,
     OBJECT_P = 547,
     OF = 548,
     OFF = 549,
     OFFSET = 550,
     OIDS = 551,
     OLD = 552,
     ON = 553,
     ONE = 554,
     ONLY = 555,
     OPERATOR = 556,
     OPTION = 557,
     OPTIONS = 558,
     OR = 559,
     ORDER = 560,
     ORDINALITY = 561,
     OTHERS = 562,
     OUT_P = 563,
     OUTER_P = 564,
     OVER = 565,
     OVERLAPS = 566,
     OVERLAY = 567,
     OVERRIDING = 568,
     OWNED = 569,
     OWNER = 570,
     PARALLEL = 571,
     PARSER = 572,
     PARTIAL = 573,
     PARTITION = 574,
     PARTITIONED = 575,
     PASSING = 576,
     PASSWORD = 577,
     PAST = 578,
     PATTERN = 579,
     PER = 580,
     PERCENT = 581,
     PERSISTENT = 582,
     PIVOT = 583,
     PIVOT_LONGER = 584,
     PIVOT_WIDER = 585,
     PLACING = 586,
     PLANS = 587,
     POLICY = 588,
     POSITION = 589,
     POSITIONAL = 590,
     PRAGMA_P = 591,
     PRECEDING = 592,
     PRECISION = 593,
     PREPARE = 594,
     PREPARED = 595,
     PRESERVE = 596,
     PRIMARY = 597,
     PRIOR = 598,
     PRIVILEGES = 599,
     PROCEDURAL = 600,
     PROCEDURE = 601,
     PROGRAM = 602,
     PUBLICATION = 603,
     QUALIFY = 604,
     QUARTER_P = 605,
     QUARTERS_P = 606,
     QUOTE = 607,
     RANGE = 608,
     READ_P = 609,
     REAL = 610,
     REASSIGN = 611,
     RECHECK = 612,
     RECURSIVE = 613,
     REF = 614,
     REFERENCES = 615,
     REFERENCING = 616,
     REFRESH = 617,
     REINDEX = 618,
     RELATIVE_P = 619,
     RELEASE = 620,
     RENAME = 621,
     REPEATABLE = 622,
     REPLACE = 623,
     REPLICA = 624,
     RESET = 625,
     RESPECT_P = 626,
     RESTART = 627,
     RESTRICT = 628,
     RETURNING = 629,
     RETURNS = 630,
     REVOKE = 631,
     RIGHT = 632,
     ROLE = 633,
     ROLLBACK = 634,
     ROLLUP = 635,
     ROW = 636,
     ROWS = 637,
     RULE = 638,
     SAMPLE = 639,
     SAVEPOINT = 640,
     SCHEMA = 641,
     SCHEMAS = 642,
     SCOPE = 643,
     SCROLL = 644,
     SEARCH = 645,
     SECOND_P = 646,
     SECONDS_P = 647,
     SECRET = 648,
     SECURITY = 649,
     SELECT = 650,
     SEMI = 651,
     SEQUENCE = 652,
     SEQUENCES = 653,
     SERIALIZABLE = 654,
     SERVER = 655,
     SESSION = 656,
     SET = 657,
     SETOF = 658,
     SETS = 659,
     SHARE = 660,
     SHOW = 661,
     SIMILAR = 662,
     SIMPLE = 663,
     SKIP = 664,
     SMALLINT = 665,
     SNAPSHOT = 666,
     SOME = 667,
     SORTED = 668,
     SOURCE_P = 669,
     SQL_P = 670,
     STABLE = 671,
     STANDALONE_P = 672,
     START = 673,
     STATEMENT = 674,
     STATISTICS = 675,
     STDIN = 676,
     STDOUT = 677,
     STORAGE = 678,
     STORED = 679,
     STRICT_P = 680,
     STRIP_P = 681,
     STRUCT = 682,
     SUBSCRIPTION = 683,
     SUBSTRING = 684,
     SUMMARIZE = 685,
     SYMMETRIC = 686,
     SYSID = 687,
     SYSTEM_P = 688,
     TABLE = 689,
     TABLES = 690,
     TABLESAMPLE = 691,
     TABLESPACE = 692,
     TARGET_P = 693,
     TEMP = 694,
     TEMPLATE = 695,
     TEMPORARY = 696,
     TEXT_P = 697,
     THEN = 698,
     TIES = 699,
     TIME = 700,
     TIMESTAMP = 701,
     TO = 702,
     TRAILING = 703,
     TRANSACTION = 704,
     TRANSFORM = 705,
     TREAT = 706,
     TRIGGER = 707,
     TRIM = 708,
     TRUE_P = 709,
     TRUNCATE = 710,
     TRUSTED = 711,
     TRY_CAST = 712,
     TYPE_P = 713,
     TYPES_P = 714,
     UNBOUNDED = 715,
     UNCOMMITTED = 716,
     UNENCRYPTED = 717,
     UNION = 718,
     UNIQUE = 719,
     UNKNOWN = 720,
     UNLISTEN = 721,
     UNLOGGED = 722,
     UNPACK = 723,
     UNPIVOT = 724,
     UNTIL = 725,
     UPDATE = 726,
     USE_P = 727,
     USER = 728,
     USING = 729,
     VACUUM = 730,
     VALID = 731,
     VALIDATE = 732,
     VALIDATOR = 733,
     VALUE_P = 734,
     VALUES = 735,
     VARCHAR = 736,
     VARIABLE_P = 737,
     VARIADIC = 738,
     VARYING = 739,
     VERBOSE = 740,
     VERSION_P = 741,
     VIEW = 742,
     VIEWS = 743,
     VIRTUAL = 744,
     VOLATILE = 745,
     WEEK_P = 746,
     WEEKS_P = 747,
     WHEN = 748,
     WHERE = 749,
     WHITESPACE_P = 750,
     WINDOW = 751,
     WITH = 752,
     WITHIN = 753,
     WITHOUT = 754,
     WORK = 755,
     WRAPPER = 756,
     WRITE_P = 757,
     XML_P = 758,
     XMLATTRIBUTES = 759,
     XMLCONCAT = 760,
     XMLELEMENT = 761,
     XMLEXISTS = 762,
     XMLFOREST = 763,
     XMLNAMESPACES = 764,
     XMLPARSE = 765,
     XMLPI = 766,
     XMLROOT = 767,
     XMLSERIALIZE = 768,
     XMLTABLE = 769,
     YEAR_P = 770,
     YEARS_P = 771,
     YES_P = 772,
     ZONE = 773,
     NOT_LA = 774,
     NULLS_LA = 775,
     WITH_LA = 776,
     POSTFIXOP = 777,
     UMINUS = 778
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
#define SINGLE_ARROW 272
#define DOUBLE_ARROW 273
#define SINGLE_COLON 274
#define LESS_EQUALS 275
#define GREATER_EQUALS 276
#define NOT_EQUALS 277
#define ABORT_P 278
#define ABSOLUTE_P 279
#define ACCESS 280
#define ACTION 281
#define ADD_P 282
#define ADMIN 283
#define AFTER 284
#define AGGREGATE 285
#define ALL 286
#define ALSO 287
#define ALTER 288
#define ALWAYS 289
#define ANALYSE 290
#define ANALYZE 291
#define AND 292
#define ANTI 293
#define ANY 294
#define ARRAY 295
#define AS 296
#define ASC_P 297
#define ASOF 298
#define ASSERTION 299
#define ASSIGNMENT 300
#define ASYMMETRIC 301
#define AT 302
#define ATTACH 303
#define ATTRIBUTE 304
#define AUTHORIZATION 305
#define BACKWARD 306
#define BEFORE 307
#define BEGIN_P 308
#define BETWEEN 309
#define BIGINT 310
#define BINARY 311
#define BIT 312
#define BOOLEAN_P 313
#define BOTH 314
#define BY 315
#define CACHE 316
#define CALL_P 317
#define CALLED 318
#define CASCADE 319
#define CASCADED 320
#define CASE 321
#define CAST 322
#define CATALOG_P 323
#define CENTURIES_P 324
#define CENTURY_P 325
#define CHAIN 326
#define CHAR_P 327
#define CHARACTER 328
#define CHARACTERISTICS 329
#define CHECK_P 330
#define CHECKPOINT 331
#define CLASS 332
#define CLOSE 333
#define CLUSTER 334
#define COALESCE 335
#define COLLATE 336
#define COLLATION 337
#define COLUMN 338
#define COLUMNS 339
#define COMMENT 340
#define COMMENTS 341
#define COMMIT 342
#define COMMITTED 343
#define COMPRESSION 344
#define CONCURRENTLY 345
#define CONFIGURATION 346
#define CONFLICT 347
#define CONNECTION 348
#define CONSTRAINT 349
#define CONSTRAINTS 350
#define CONTENT_P 351
#define CONTINUE_P 352
#define CONVERSION_P 353
#define COPY 354
#define COST 355
#define CREATE_P 356
#define CROSS 357
#define CSV 358
#define CUBE 359
#define CURRENT_P 360
#define CURSOR 361
#define CYCLE 362
#define DATA_P 363
#define DATABASE 364
#define DAY_P 365
#define DAYS_P 366
#define DEALLOCATE 367
#define DEC 368
#define DECADE_P 369
#define DECADES_P 370
#define DECIMAL_P 371
#define DECLARE 372
#define DEFAULT 373
#define DEFAULTS 374
#define DEFERRABLE 375
#define DEFERRED 376
#define DEFINE 377
#define DEFINER 378
#define DELETE_P 379
#define DELIMITER 380
#define DELIMITERS 381
#define DEPENDS 382
#define DESC_P 383
#define DESCRIBE 384
#define DETACH 385
#define DICTIONARY 386
#define DISABLE_P 387
#define DISCARD 388
#define DISTINCT 389
#define DO 390
#define DOCUMENT_P 391
#define DOMAIN_P 392
#define DOUBLE_P 393
#define DROP 394
#define EACH 395
#define ELSE 396
#define ENABLE_P 397
#define ENCODING 398
#define ENCRYPTED 399
#define END_P 400
#define ENUM_P 401
#define ERROR_P 402
#define ESCAPE 403
#define EVENT 404
#define EXCEPT 405
#define EXCLUDE 406
#define EXCLUDING 407
#define EXCLUSIVE 408
#define EXECUTE 409
#define EXISTS 410
#define EXPLAIN 411
#define EXPORT_P 412
#define EXPORT_STATE 413
#define EXTENSION 414
#define EXTENSIONS 415
#define EXTERNAL 416
#define EXTRACT 417
#define FALSE_P 418
#define FAMILY 419
#define FETCH 420
#define FILTER 421
#define FIRST_P 422
#define FLOAT_P 423
#define FOLLOWING 424
#define FOR 425
#define FORCE 426
#define FOREIGN 427
#define FORWARD 428
#define FREEZE 429
#define FROM 430
#define FULL 431
#define FUNCTION 432
#define FUNCTIONS 433
#define GENERATED 434
#define GLOB 435
#define GLOBAL 436
#define GRANT 437
#define GRANTED 438
#define GROUP_P 439
#define GROUPING 440
#define GROUPING_ID 441
#define GROUPS 442
#define HANDLER 443
#define HAVING 444
#define HEADER_P 445
#define HOLD 446
#define HOUR_P 447
#define HOURS_P 448
#define IDENTITY_P 449
#define IF_P 450
#define IGNORE_P 451
#define ILIKE 452
#define IMMEDIATE 453
#define IMMUTABLE 454
#define IMPLICIT_P 455
#define IMPORT_P 456
#define IN_P 457
#define INCLUDE_P 458
#define INCLUDING 459
#define INCREMENT 460
#define INDEX 461
#define INDEXES 462
#define INHERIT 463
#define INHERITS 464
#define INITIALLY 465
#define INLINE_P 466
#define INNER_P 467
#define INOUT 468
#define INPUT_P 469
#define INSENSITIVE 470
#define INSERT 471
#define INSTALL 472
#define INSTEAD 473
#define INT_P 474
#define INTEGER 475
#define INTERSECT 476
#define INTERVAL 477
#define INTO 478
#define INVOKER 479
#define IS 480
#define ISNULL 481
#define ISOLATION 482
#define JOIN 483
#define JSON 484
#define KEY 485
#define LABEL 486
#define LAMBDA 487
#define LANGUAGE 488
#define LARGE_P 489
#define LAST_P 490
#define LATERAL_P 491
#define LEADING 492
#define LEAKPROOF 493
#define LEFT 494
#define LEVEL 495
#define LIKE 496
#define LIMIT 497
#define LISTEN 498
#define LOAD 499
#define LOCAL 500
#define LOCATION 501
#define LOCK_P 502
#define LOCKED 503
#define LOGGED 504
#define MACRO 505
#define MAP 506
#define MAPPING 507
#define MATCH 508
#define MATCH_RECOGNIZE 509
#define MATCHED 510
#define MATERIALIZED 511
#define MAXVALUE 512
#define MEASURES 513
#define MERGE 514
#define METHOD 515
#define MICROSECOND_P 516
#define MICROSECONDS_P 517
#define MILLENNIA_P 518
#define MILLENNIUM_P 519
#define MILLISECOND_P 520
#define MILLISECONDS_P 521
#define MINUTE_P 522
#define MINUTES_P 523
#define MINVALUE 524
#define MODE 525
#define MONTH_P 526
#define MONTHS_P 527
#define MOVE 528
#define NAME_P 529
#define NAMES 530
#define NATIONAL 531
#define NATURAL 532
#define NCHAR 533
#define NEW 534
#define NEXT 535
#define NO 536
#define NONE 537
#define NOT 538
#define NOTHING 539
#define NOTIFY 540
#define NOTNULL 541
#define NOWAIT 542
#define NULL_P 543
#define NULLIF 544
#define NULLS_P 545
#define NUMERIC 546
#define OBJECT_P 547
#define OF 548
#define OFF 549
#define OFFSET 550
#define OIDS 551
#define OLD 552
#define ON 553
#define ONE 554
#define ONLY 555
#define OPERATOR 556
#define OPTION 557
#define OPTIONS 558
#define OR 559
#define ORDER 560
#define ORDINALITY 561
#define OTHERS 562
#define OUT_P 563
#define OUTER_P 564
#define OVER 565
#define OVERLAPS 566
#define OVERLAY 567
#define OVERRIDING 568
#define OWNED 569
#define OWNER 570
#define PARALLEL 571
#define PARSER 572
#define PARTIAL 573
#define PARTITION 574
#define PARTITIONED 575
#define PASSING 576
#define PASSWORD 577
#define PAST 578
#define PATTERN 579
#define PER 580
#define PERCENT 581
#define PERSISTENT 582
#define PIVOT 583
#define PIVOT_LONGER 584
#define PIVOT_WIDER 585
#define PLACING 586
#define PLANS 587
#define POLICY 588
#define POSITION 589
#define POSITIONAL 590
#define PRAGMA_P 591
#define PRECEDING 592
#define PRECISION 593
#define PREPARE 594
#define PREPARED 595
#define PRESERVE 596
#define PRIMARY 597
#define PRIOR 598
#define PRIVILEGES 599
#define PROCEDURAL 600
#define PROCEDURE 601
#define PROGRAM 602
#define PUBLICATION 603
#define QUALIFY 604
#define QUARTER_P 605
#define QUARTERS_P 606
#define QUOTE 607
#define RANGE 608
#define READ_P 609
#define REAL 610
#define REASSIGN 611
#define RECHECK 612
#define RECURSIVE 613
#define REF 614
#define REFERENCES 615
#define REFERENCING 616
#define REFRESH 617
#define REINDEX 618
#define RELATIVE_P 619
#define RELEASE 620
#define RENAME 621
#define REPEATABLE 622
#define REPLACE 623
#define REPLICA 624
#define RESET 625
#define RESPECT_P 626
#define RESTART 627
#define RESTRICT 628
#define RETURNING 629
#define RETURNS 630
#define REVOKE 631
#define RIGHT 632
#define ROLE 633
#define ROLLBACK 634
#define ROLLUP 635
#define ROW 636
#define ROWS 637
#define RULE 638
#define SAMPLE 639
#define SAVEPOINT 640
#define SCHEMA 641
#define SCHEMAS 642
#define SCOPE 643
#define SCROLL 644
#define SEARCH 645
#define SECOND_P 646
#define SECONDS_P 647
#define SECRET 648
#define SECURITY 649
#define SELECT 650
#define SEMI 651
#define SEQUENCE 652
#define SEQUENCES 653
#define SERIALIZABLE 654
#define SERVER 655
#define SESSION 656
#define SET 657
#define SETOF 658
#define SETS 659
#define SHARE 660
#define SHOW 661
#define SIMILAR 662
#define SIMPLE 663
#define SKIP 664
#define SMALLINT 665
#define SNAPSHOT 666
#define SOME 667
#define SORTED 668
#define SOURCE_P 669
#define SQL_P 670
#define STABLE 671
#define STANDALONE_P 672
#define START 673
#define STATEMENT 674
#define STATISTICS 675
#define STDIN 676
#define STDOUT 677
#define STORAGE 678
#define STORED 679
#define STRICT_P 680
#define STRIP_P 681
#define STRUCT 682
#define SUBSCRIPTION 683
#define SUBSTRING 684
#define SUMMARIZE 685
#define SYMMETRIC 686
#define SYSID 687
#define SYSTEM_P 688
#define TABLE 689
#define TABLES 690
#define TABLESAMPLE 691
#define TABLESPACE 692
#define TARGET_P 693
#define TEMP 694
#define TEMPLATE 695
#define TEMPORARY 696
#define TEXT_P 697
#define THEN 698
#define TIES 699
#define TIME 700
#define TIMESTAMP 701
#define TO 702
#define TRAILING 703
#define TRANSACTION 704
#define TRANSFORM 705
#define TREAT 706
#define TRIGGER 707
#define TRIM 708
#define TRUE_P 709
#define TRUNCATE 710
#define TRUSTED 711
#define TRY_CAST 712
#define TYPE_P 713
#define TYPES_P 714
#define UNBOUNDED 715
#define UNCOMMITTED 716
#define UNENCRYPTED 717
#define UNION 718
#define UNIQUE 719
#define UNKNOWN 720
#define UNLISTEN 721
#define UNLOGGED 722
#define UNPACK 723
#define UNPIVOT 724
#define UNTIL 725
#define UPDATE 726
#define USE_P 727
#define USER 728
#define USING 729
#define VACUUM 730
#define VALID 731
#define VALIDATE 732
#define VALIDATOR 733
#define VALUE_P 734
#define VALUES 735
#define VARCHAR 736
#define VARIABLE_P 737
#define VARIADIC 738
#define VARYING 739
#define VERBOSE 740
#define VERSION_P 741
#define VIEW 742
#define VIEWS 743
#define VIRTUAL 744
#define VOLATILE 745
#define WEEK_P 746
#define WEEKS_P 747
#define WHEN 748
#define WHERE 749
#define WHITESPACE_P 750
#define WINDOW 751
#define WITH 752
#define WITHIN 753
#define WITHOUT 754
#define WORK 755
#define WRAPPER 756
#define WRITE_P 757
#define XML_P 758
#define XMLATTRIBUTES 759
#define XMLCONCAT 760
#define XMLELEMENT 761
#define XMLEXISTS 762
#define XMLFOREST 763
#define XMLNAMESPACES 764
#define XMLPARSE 765
#define XMLPI 766
#define XMLROOT 767
#define XMLSERIALIZE 768
#define XMLTABLE 769
#define YEAR_P 770
#define YEARS_P 771
#define YES_P 772
#define ZONE 773
#define NOT_LA 774
#define NULLS_LA 775
#define WITH_LA 776
#define POSTFIXOP 777
#define UMINUS 778




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
	PGMergeAction mergeaction;
}
/* Line 1529 of yacc.c.  */
#line 1149 "third_party/libpg_query/grammar/grammar_out.hpp"
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


