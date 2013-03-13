/**
 * @file Unit tests for SomaticEvent subclasses
 *
 * @see SomaticEvent
 * @author Yi Qiao
 */

#include <iostream>
#include <boost/test/unit_test.hpp>
#include "../SomaticEvent.h"
#include "../SegmentalMutation.h"
#include "../SNP.h"
#include <sqlite3/sqlite3.h>
#include <cstdio>

void testCNV() {
	SubcloneExplorer::CNV cnv;

	BOOST_CHECK_CLOSE( cnv.frequency, 0, 1e-3);
	BOOST_CHECK(cnv.range.chrom==0);
	BOOST_CHECK(cnv.range.position==0);
	BOOST_CHECK(cnv.range.length==0);

	cnv.frequency=0.2;
	cnv.range.chrom=1;
	cnv.range.position = 1000000L;
	cnv.range.length = 1000L;

	BOOST_CHECK_CLOSE(cnv.frequency, 0.2, 1e-3);
	BOOST_CHECK(cnv.range.chrom==1);
	BOOST_CHECK(cnv.range.position==1000000L);
	BOOST_CHECK(cnv.range.length==1000L);

	SubcloneExplorer::CNV *cnv2 = new SubcloneExplorer::CNV();
	cnv2->frequency = 0.4;
	cnv2->range.chrom=1;
	cnv2->range.position = 1000123L;
	cnv2->range.length = 1012L;

	BOOST_CHECK(cnv.isEqualTo(cnv2));

	SubcloneExplorer::LOH *loh = new SubcloneExplorer::LOH();
	loh->frequency = 0.4;
	loh->range.chrom=1;
	loh->range.position = 1000123L;
	loh->range.length = 1012L;

	BOOST_CHECK(not cnv.isEqualTo(loh));


}

void testCNVToDB() {
	SubcloneExplorer::CNV cnv;

	cnv.frequency=0.2;
	cnv.range.chrom=1;
	cnv.range.position = 1000000L;
	cnv.range.length = 1000L;

	BOOST_CHECK_CLOSE(cnv.frequency, 0.2, 1e-3);
	BOOST_CHECK(cnv.range.chrom==1);
	BOOST_CHECK(cnv.range.position==1000000L);
	BOOST_CHECK(cnv.range.length==1000L);

	// write
	sqlite3 *database;
	sqlite3_open("test.sqlite", &database);

	cnv.createTableInDB(database);
	sqlite3_int64 id = cnv.archiveObjectToDB(database);

	BOOST_CHECK(id != 0);

	sqlite3_close(database);
	database = 0;

	// read
	SubcloneExplorer::CNV cnv2;
	sqlite3_open("test.sqlite", &database);

	bool status = cnv2.unarchiveObjectFromDB(database, id);

	BOOST_CHECK(status);
	BOOST_CHECK_CLOSE(cnv2.frequency, 0.2, 1e-3);
	BOOST_CHECK(cnv2.range.chrom==1);
	BOOST_CHECK(cnv2.range.position==1000000L);
	BOOST_CHECK(cnv2.range.length==1000L);

#ifndef KEEP_TEST_DB
	remove("test.sqlite");
#endif
}

void testLOH() {
	SubcloneExplorer::LOH loh;
	BOOST_CHECK_CLOSE( loh.frequency, 0, 1e-3);
	BOOST_CHECK(loh.range.chrom==0);
	BOOST_CHECK(loh.range.position==0);
	BOOST_CHECK(loh.range.length==0);

	loh.frequency=0.2;
	loh.range.chrom=1;
	loh.range.position = 1000000L;
	loh.range.length = 1000L;

	BOOST_CHECK_CLOSE(loh.frequency, 0.2, 1e-3);
	BOOST_CHECK(loh.range.chrom==1);
	BOOST_CHECK(loh.range.position==1000000L);
	BOOST_CHECK(loh.range.length==1000L);
}

void testLOHToDB() {
	SubcloneExplorer::LOH loh;

	loh.frequency=0.2;
	loh.range.chrom=1;
	loh.range.position = 1000000L;
	loh.range.length = 1000L;

	BOOST_CHECK_CLOSE(loh.frequency, 0.2, 1e-3);
	BOOST_CHECK(loh.range.chrom==1);
	BOOST_CHECK(loh.range.position==1000000L);
	BOOST_CHECK(loh.range.length==1000L);

	// write
	sqlite3 *database;
	sqlite3_open("test.sqlite", &database);

	loh.createTableInDB(database);
	sqlite3_int64 id = loh.archiveObjectToDB(database);

	BOOST_CHECK(id != 0);

	sqlite3_close(database);
	database = 0;

	// read
	SubcloneExplorer::LOH loh2;
	sqlite3_open("test.sqlite", &database);

	bool status = loh2.unarchiveObjectFromDB(database, id);

	BOOST_CHECK(status);
	BOOST_CHECK_CLOSE(loh2.frequency, 0.2, 1e-3);
	BOOST_CHECK(loh2.range.chrom==1);
	BOOST_CHECK(loh2.range.position==1000000L);
	BOOST_CHECK(loh2.range.length==1000L);

#ifndef KEEP_TEST_DB
	remove("test.sqlite");
#endif
}

void testSNP() {
	SubcloneExplorer::SNP snp;

	BOOST_CHECK_CLOSE(snp.frequency, 0, 1e-3);
	BOOST_CHECK(snp.location.chrom==0);
	BOOST_CHECK(snp.location.position==0);


	snp.frequency=0.2;
	snp.location.chrom=1;
	snp.location.position = 1000000L;


	BOOST_CHECK_CLOSE(snp.frequency, 0.2, 1e-3);
	BOOST_CHECK(snp.location.chrom==1);
	BOOST_CHECK(snp.location.position==1000000L);

}

void testSNPToDB() {
	SubcloneExplorer::SNP snp;

	snp.frequency=0.2;
	snp.location.chrom=1;
	snp.location.position = 1000000L;

	BOOST_CHECK_CLOSE(snp.frequency, 0.2, 1e-3);
	BOOST_CHECK(snp.location.chrom==1);
	BOOST_CHECK(snp.location.position==1000000L);

	// write
	sqlite3 *database;
	sqlite3_open("test.sqlite", &database);

	snp.createTableInDB(database);
	sqlite3_int64 id = snp.archiveObjectToDB(database);

	BOOST_CHECK(id != 0);

	sqlite3_close(database);
	database = 0;

	// read
	SubcloneExplorer::SNP snp2;
	sqlite3_open("test.sqlite", &database);

	bool status = snp2.unarchiveObjectFromDB(database, id);

	BOOST_CHECK(status);
	BOOST_CHECK_CLOSE(snp2.frequency, 0.2, 1e-3);
	BOOST_CHECK(snp2.location.chrom==1);
	BOOST_CHECK(snp2.location.position==1000000L);

#ifndef KEEP_TEST_DB
	remove("test.sqlite");
#endif
}
