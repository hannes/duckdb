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
     PATTERN = 573,
     PER = 574,
     PERCENT = 575,
     PERSISTENT = 576,
     PIVOT = 577,
     PIVOT_LONGER = 578,
     PIVOT_WIDER = 579,
     PLACING = 580,
     PLANS = 581,
     POLICY = 582,
     POSITION = 583,
     POSITIONAL = 584,
     PRAGMA_P = 585,
     PRECEDING = 586,
     PRECISION = 587,
     PREPARE = 588,
     PREPARED = 589,
     PRESERVE = 590,
     PRIMARY = 591,
     PRIOR = 592,
     PRIVILEGES = 593,
     PROCEDURAL = 594,
     PROCEDURE = 595,
     PROGRAM = 596,
     PUBLICATION = 597,
     QUALIFY = 598,
     QUARTER_P = 599,
     QUARTERS_P = 600,
     QUOTE = 601,
     RANGE = 602,
     READ_P = 603,
     REAL = 604,
     REASSIGN = 605,
     RECHECK = 606,
     RECURSIVE = 607,
     REF = 608,
     REFERENCES = 609,
     REFERENCING = 610,
     REFRESH = 611,
     REINDEX = 612,
     RELATIVE_P = 613,
     RELEASE = 614,
     RENAME = 615,
     REPEATABLE = 616,
     REPLACE = 617,
     REPLICA = 618,
     RESET = 619,
     RESPECT_P = 620,
     RESTART = 621,
     RESTRICT = 622,
     RETURNING = 623,
     RETURNS = 624,
     REVOKE = 625,
     RIGHT = 626,
     ROLE = 627,
     ROLLBACK = 628,
     ROLLUP = 629,
     ROW = 630,
     ROWS = 631,
     RULE = 632,
     SAMPLE = 633,
     SAVEPOINT = 634,
     SCHEMA = 635,
     SCHEMAS = 636,
     SCOPE = 637,
     SCROLL = 638,
     SEARCH = 639,
     SECOND_P = 640,
     SECONDS_P = 641,
     SECRET = 642,
     SECURITY = 643,
     SELECT = 644,
     SEMI = 645,
     SEQUENCE = 646,
     SEQUENCES = 647,
     SERIALIZABLE = 648,
     SERVER = 649,
     SESSION = 650,
     SET = 651,
     SETOF = 652,
     SETS = 653,
     SHARE = 654,
     SHOW = 655,
     SIMILAR = 656,
     SIMPLE = 657,
     SKIP = 658,
     SMALLINT = 659,
     SNAPSHOT = 660,
     SOME = 661,
     SORTED = 662,
     SQL_P = 663,
     STABLE = 664,
     STANDALONE_P = 665,
     START = 666,
     STATEMENT = 667,
     STATISTICS = 668,
     STDIN = 669,
     STDOUT = 670,
     STORAGE = 671,
     STORED = 672,
     STRICT_P = 673,
     STRIP_P = 674,
     STRUCT = 675,
     SUBSCRIPTION = 676,
     SUBSTRING = 677,
     SUMMARIZE = 678,
     SYMMETRIC = 679,
     SYSID = 680,
     SYSTEM_P = 681,
     TABLE = 682,
     TABLES = 683,
     TABLESAMPLE = 684,
     TABLESPACE = 685,
     TEMP = 686,
     TEMPLATE = 687,
     TEMPORARY = 688,
     TEXT_P = 689,
     THEN = 690,
     TIES = 691,
     TIME = 692,
     TIMESTAMP = 693,
     TO = 694,
     TRAILING = 695,
     TRANSACTION = 696,
     TRANSFORM = 697,
     TREAT = 698,
     TRIGGER = 699,
     TRIM = 700,
     TRUE_P = 701,
     TRUNCATE = 702,
     TRUSTED = 703,
     TRY_CAST = 704,
     TYPE_P = 705,
     TYPES_P = 706,
     UNBOUNDED = 707,
     UNCOMMITTED = 708,
     UNENCRYPTED = 709,
     UNION = 710,
     UNIQUE = 711,
     UNKNOWN = 712,
     UNLISTEN = 713,
     UNLOGGED = 714,
     UNPACK = 715,
     UNPIVOT = 716,
     UNTIL = 717,
     UPDATE = 718,
     USE_P = 719,
     USER = 720,
     USING = 721,
     VACUUM = 722,
     VALID = 723,
     VALIDATE = 724,
     VALIDATOR = 725,
     VALUE_P = 726,
     VALUES = 727,
     VARCHAR = 728,
     VARIABLE_P = 729,
     VARIADIC = 730,
     VARYING = 731,
     VERBOSE = 732,
     VERSION_P = 733,
     VIEW = 734,
     VIEWS = 735,
     VIRTUAL = 736,
     VOLATILE = 737,
     WEEK_P = 738,
     WEEKS_P = 739,
     WHEN = 740,
     WHERE = 741,
     WHITESPACE_P = 742,
     WINDOW = 743,
     WITH = 744,
     WITHIN = 745,
     WITHOUT = 746,
     WORK = 747,
     WRAPPER = 748,
     WRITE_P = 749,
     XML_P = 750,
     XMLATTRIBUTES = 751,
     XMLCONCAT = 752,
     XMLELEMENT = 753,
     XMLEXISTS = 754,
     XMLFOREST = 755,
     XMLNAMESPACES = 756,
     XMLPARSE = 757,
     XMLPI = 758,
     XMLROOT = 759,
     XMLSERIALIZE = 760,
     XMLTABLE = 761,
     YEAR_P = 762,
     YEARS_P = 763,
     YES_P = 764,
     ZONE = 765,
     NOT_LA = 766,
     NULLS_LA = 767,
     WITH_LA = 768,
     POSTFIXOP = 769,
     UMINUS = 770
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
#define PATTERN 573
#define PER 574
#define PERCENT 575
#define PERSISTENT 576
#define PIVOT 577
#define PIVOT_LONGER 578
#define PIVOT_WIDER 579
#define PLACING 580
#define PLANS 581
#define POLICY 582
#define POSITION 583
#define POSITIONAL 584
#define PRAGMA_P 585
#define PRECEDING 586
#define PRECISION 587
#define PREPARE 588
#define PREPARED 589
#define PRESERVE 590
#define PRIMARY 591
#define PRIOR 592
#define PRIVILEGES 593
#define PROCEDURAL 594
#define PROCEDURE 595
#define PROGRAM 596
#define PUBLICATION 597
#define QUALIFY 598
#define QUARTER_P 599
#define QUARTERS_P 600
#define QUOTE 601
#define RANGE 602
#define READ_P 603
#define REAL 604
#define REASSIGN 605
#define RECHECK 606
#define RECURSIVE 607
#define REF 608
#define REFERENCES 609
#define REFERENCING 610
#define REFRESH 611
#define REINDEX 612
#define RELATIVE_P 613
#define RELEASE 614
#define RENAME 615
#define REPEATABLE 616
#define REPLACE 617
#define REPLICA 618
#define RESET 619
#define RESPECT_P 620
#define RESTART 621
#define RESTRICT 622
#define RETURNING 623
#define RETURNS 624
#define REVOKE 625
#define RIGHT 626
#define ROLE 627
#define ROLLBACK 628
#define ROLLUP 629
#define ROW 630
#define ROWS 631
#define RULE 632
#define SAMPLE 633
#define SAVEPOINT 634
#define SCHEMA 635
#define SCHEMAS 636
#define SCOPE 637
#define SCROLL 638
#define SEARCH 639
#define SECOND_P 640
#define SECONDS_P 641
#define SECRET 642
#define SECURITY 643
#define SELECT 644
#define SEMI 645
#define SEQUENCE 646
#define SEQUENCES 647
#define SERIALIZABLE 648
#define SERVER 649
#define SESSION 650
#define SET 651
#define SETOF 652
#define SETS 653
#define SHARE 654
#define SHOW 655
#define SIMILAR 656
#define SIMPLE 657
#define SKIP 658
#define SMALLINT 659
#define SNAPSHOT 660
#define SOME 661
#define SORTED 662
#define SQL_P 663
#define STABLE 664
#define STANDALONE_P 665
#define START 666
#define STATEMENT 667
#define STATISTICS 668
#define STDIN 669
#define STDOUT 670
#define STORAGE 671
#define STORED 672
#define STRICT_P 673
#define STRIP_P 674
#define STRUCT 675
#define SUBSCRIPTION 676
#define SUBSTRING 677
#define SUMMARIZE 678
#define SYMMETRIC 679
#define SYSID 680
#define SYSTEM_P 681
#define TABLE 682
#define TABLES 683
#define TABLESAMPLE 684
#define TABLESPACE 685
#define TEMP 686
#define TEMPLATE 687
#define TEMPORARY 688
#define TEXT_P 689
#define THEN 690
#define TIES 691
#define TIME 692
#define TIMESTAMP 693
#define TO 694
#define TRAILING 695
#define TRANSACTION 696
#define TRANSFORM 697
#define TREAT 698
#define TRIGGER 699
#define TRIM 700
#define TRUE_P 701
#define TRUNCATE 702
#define TRUSTED 703
#define TRY_CAST 704
#define TYPE_P 705
#define TYPES_P 706
#define UNBOUNDED 707
#define UNCOMMITTED 708
#define UNENCRYPTED 709
#define UNION 710
#define UNIQUE 711
#define UNKNOWN 712
#define UNLISTEN 713
#define UNLOGGED 714
#define UNPACK 715
#define UNPIVOT 716
#define UNTIL 717
#define UPDATE 718
#define USE_P 719
#define USER 720
#define USING 721
#define VACUUM 722
#define VALID 723
#define VALIDATE 724
#define VALIDATOR 725
#define VALUE_P 726
#define VALUES 727
#define VARCHAR 728
#define VARIABLE_P 729
#define VARIADIC 730
#define VARYING 731
#define VERBOSE 732
#define VERSION_P 733
#define VIEW 734
#define VIEWS 735
#define VIRTUAL 736
#define VOLATILE 737
#define WEEK_P 738
#define WEEKS_P 739
#define WHEN 740
#define WHERE 741
#define WHITESPACE_P 742
#define WINDOW 743
#define WITH 744
#define WITHIN 745
#define WITHOUT 746
#define WORK 747
#define WRAPPER 748
#define WRITE_P 749
#define XML_P 750
#define XMLATTRIBUTES 751
#define XMLCONCAT 752
#define XMLELEMENT 753
#define XMLEXISTS 754
#define XMLFOREST 755
#define XMLNAMESPACES 756
#define XMLPARSE 757
#define XMLPI 758
#define XMLROOT 759
#define XMLSERIALIZE 760
#define XMLTABLE 761
#define YEAR_P 762
#define YEARS_P 763
#define YES_P 764
#define ZONE 765
#define NOT_LA 766
#define NULLS_LA 767
#define WITH_LA 768
#define POSTFIXOP 769
#define UMINUS 770




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
#line 1131 "third_party/libpg_query/grammar/grammar_out.hpp"
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


