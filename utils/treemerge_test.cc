/**
 * @file treemerge_test.cc
 * Test cases for treemerge logics
 *
 * @author Yi Qiao
 */

#include <UnitTest++/src/UnitTest++.h>
#include <algorithm>
#include <vector>
#include "treemerge_p.h"

#include "SomaticEvent.h"
#include "SegmentalMutation.h"
#include "EventCluster.h"
#include "TreeNode.h"
#include "Subclone.h"

using namespace SubcloneExplorer;

struct _SampleSubclonesFixture {

	Subclone * tumor1;

	_SampleSubclonesFixture() {
		CNV a, b, c, d, e;
		a.range.chrom = 1; b.range.chrom = 2; c.range.chrom = 3; d.range.chrom = 4; e.range.chrom = 5;

		EventCluster cA, cB, cC, cD, cE;
		cA.addEvent(&a); cB.addEvent(&b); cC.addEvent(&c); cD.addEvent(&d), cE.addEvent(&e);

		Subclone *p0, *p1, *p2, *p3;
		p0 = new Subclone(); p0->setFraction(0.1);
		p1 = new Subclone(); p1->setFraction(0.1); p1->addEventCluster(&cA);
		p2 = new Subclone(); p2->setFraction(0.1); p2->addEventCluster(&cB);
		p3 = new Subclone(); p3->setFraction(0.1); p3->addEventCluster(&cC);

		p0->setId(10); p1->setId(11); p2->setId(12); p3->setId(13);
		p0->addChild(p1); p1->addChild(p2); p1->addChild(p3);
		tumor1 = p0;
	}

	~_SampleSubclonesFixture() {
		delete tumor1;
	}
};

SUITE(TestNodeEventsList) {
	TEST_FIXTURE(_SampleSubclonesFixture, T_nodeEventsList) {
		Subclone *p = tumor1;
		SomaticEventPtr_vec events;
		
		events = nodeEventsList(p);
		CHECK(events.size() == 0);

		p = dynamic_cast<Subclone *>(p->getVecChildren()[0]);

		events = nodeEventsList(p);
		CHECK(events.size() == 1);

		p = dynamic_cast<Subclone *>(p->getVecChildren()[0]);

		events = nodeEventsList(p);
		CHECK(events.size() == 2);

		CHECK(p->getVecChildren().size() == 0);
	}
}

struct _SampleSomaticEventsFixture {
	SomaticEventPtr_vec v1;
	SomaticEventPtr_vec v2;
	SomaticEventPtr_vec v3;
	SomaticEventPtr_vec v4;

	CNV *a, *b, *c, *d, *e;

	_SampleSomaticEventsFixture() {
		a = new CNV(); b = new CNV(); c = new CNV(); d = new CNV(); e = new CNV();
		a->range.chrom = 1; b->range.chrom = 2; c->range.chrom = 3; d->range.chrom = 4; e->range.chrom = 5;

		v1.push_back(a); 
		v1.push_back(b);
		v1.push_back(c);

		v2.push_back(a);
		v2.push_back(d);
		v2.push_back(e);
		
		v3.push_back(d);
		v3.push_back(e);

		v4.push_back(b);
	}

	~_SampleSomaticEventsFixture() {
		delete a;
		delete b;
		delete c;
		delete d;
		delete e;
	}
};


TEST_FIXTURE(_SampleSomaticEventsFixture, T_SomaticEventDifference) {
	SomaticEventPtr_vec diff = SomaticEventDifference(v1, v2);
	CHECK(diff.size() == 2);

	bool found = true;
	found = found && std::find(diff.begin(), diff.end(), a) == diff.end();
	found = found && std::find(diff.begin(), diff.end(), b) != diff.end();
	found = found && std::find(diff.begin(), diff.end(), c) != diff.end();
	CHECK(found);

	diff = SomaticEventDifference(v1, v1);
	CHECK(diff.size() == 0);
}

TEST_FIXTURE(_SampleSomaticEventsFixture, T_eventSetContains) {

	CHECK(eventSetContains(v1, v1));

	CHECK(eventSetContains(v2, v3));
	CHECK(not eventSetContains(v1, v3));

	CHECK(not eventSetContains(v1, v2));
}

TEST_FIXTURE(_SampleSomaticEventsFixture, T_resultSetComparator) {
	std::vector<SomaticEventPtr_vec> testSets;
	testSets.push_back(v1);
	testSets.push_back(v3);
	testSets.push_back(v4);

	std::sort(testSets.begin(), testSets.end(), resultSetComparator);

	CHECK(testSets[0].size() <= testSets[1].size());
	CHECK(testSets[1].size() <= testSets[2].size());
}


#pragma mark --Sample Tumors--

SUITE(SampleTumors) {
	// 0 == 0
	TEST(test_tree_merge_0_0) {
		Subclone *clone1 = new Subclone();
		Subclone *clone2 = new Subclone();

		clone1->setFraction(1);
		clone2->setFraction(1);

		CHECK(TreeMerge(clone1, clone2));

		delete clone1;
		delete clone2;
	}

	// 0 == A
	TEST(test_tree_merge_0_a) {
		Subclone *clone1 = new Subclone();
		Subclone *clone2 = new Subclone();

		CNV cnv;
		cnv.range.chrom = 1;
		cnv.range.position = 1000000L;
		cnv.range.length = 1000;
		cnv.frequency = 0.4;

		EventCluster cluster;
		cluster.addEvent(&cnv);

		clone2->addEventCluster(&cluster);

		clone1->setFraction(1);
		clone2->setFraction(1);

		CHECK(TreeMerge(clone1, clone2));
		delete clone1;
		delete clone2;
	}

	// A == A
	TEST(test_tree_merge_a_a) {
		CNV *cnv = new CNV();
		cnv->range.chrom = 1;
		cnv->range.position = 100000L;
		cnv->range.length = 100;
		cnv->frequency = 0.4;

		EventCluster *cluster1 = new EventCluster();
		EventCluster *cluster2 = new EventCluster();

		cluster1->addEvent(cnv);
		cluster2->addEvent(cnv);

		Subclone *clone1 = new Subclone();
		Subclone *clone2 = new Subclone();
		Subclone *clone3 = new Subclone();
		Subclone *clone4 = new Subclone();

		clone3->addEventCluster(cluster1);
		clone4->addEventCluster(cluster2);

		clone1->addChild(clone3);
		clone2->addChild(clone4);

		clone1->setFraction(0.6); clone2->setFraction(0.6);
		clone3->setFraction(0.4); clone4->setFraction(0.4);

		CHECK(TreeMerge(clone1, clone2));

		delete clone1;
		delete clone2;

	}

	// A ~= A'
	TEST(test_tree_merge_a_a1) {
		CNV *cnv1 = new CNV();
		CNV *cnv2 = new CNV();
		cnv1->range.chrom = 1;
		cnv1->range.position = 100000L;
		cnv1->range.length = 1000;
		cnv1->frequency = 0.4;

		cnv2->range.chrom = 1;
		cnv2->range.position = 100123L;
		cnv2->range.length = 986;
		cnv2->frequency = 0.4;

		CHECK(cnv1->isEqualTo(cnv2, BOUNDRY_RESOLUTION));

		EventCluster *cluster1 = new EventCluster();
		EventCluster *cluster2 = new EventCluster();

		cluster1->addEvent(cnv1);
		cluster2->addEvent(cnv2);

		Subclone *clone1 = new Subclone();
		Subclone *clone2 = new Subclone();
		Subclone *clone3 = new Subclone();
		Subclone *clone4 = new Subclone();

		clone3->addEventCluster(cluster1);
		clone4->addEventCluster(cluster2);

		clone1->addChild(clone3);
		clone2->addChild(clone4);

		clone1->setFraction(0.6); clone2->setFraction(0.6);
		clone3->setFraction(0.4); clone4->setFraction(0.4);


		CHECK(TreeMerge(clone1, clone2));

		delete clone1;
		delete clone2;
		delete clone3;
		delete clone4;

	}

	// 0, (A, (B, (C))) != 0, (A, (B, C))
	TEST(test_tree_merge_3_1) {
		CNV A, B, C;
		A.range.chrom = 1;
		B.range.chrom = 2;
		C.range.chrom = 3;

		EventCluster cA, cB, cC;
		cA.addEvent(&A);
		cB.addEvent(&B);
		cC.addEvent(&C);

		Subclone *p0, *pA, *pB, *pC;
		Subclone *r0, *rA, *rB, *rC;

		p0 = new Subclone(); pA = new Subclone(); pB = new Subclone(); pC = new Subclone();
		r0 = new Subclone(); rA = new Subclone(); rB = new Subclone(); rC = new Subclone();

		p0->setId(10); pA->setId(11); pB->setId(12); pC->setId(13);
		r0->setId(20); rA->setId(21); rB->setId(22); rC->setId(23);

		pA->addEventCluster(&cA); rA->addEventCluster(&cA);
		pB->addEventCluster(&cB); rB->addEventCluster(&cB);
		pC->addEventCluster(&cC); rC->addEventCluster(&cC);

		p0->addChild(pA); pA->addChild(pB); pB->addChild(pC);
		r0->addChild(rA); rA->addChild(rB); rA->addChild(rC);

		p0->setFraction(0.1); pA->setFraction(0.1); pB->setFraction(0.1); pC->setFraction(0.1);
		r0->setFraction(0.1); rA->setFraction(0.1); rB->setFraction(0.1); rC->setFraction(0.1);

		CHECK(not TreeMerge(p0, r0));
	}

	// 0, (A, (B, (C))) == 0, (A, (B, D))
	TEST(test_tree_merge_3_2) {
		CNV A, B, C, D;
		A.range.chrom = 1;
		B.range.chrom = 2;
		C.range.chrom = 3;
		D.range.chrom = 4;

		EventCluster cA, cB, cC, cD;
		cA.addEvent(&A);
		cB.addEvent(&B);
		cC.addEvent(&C);
		cD.addEvent(&D);

		Subclone *p0, *pA, *pB, *pC;
		Subclone *r0, *rA, *rB, *rD;

		p0 = new Subclone(); pA = new Subclone(); pB = new Subclone(); pC = new Subclone();
		r0 = new Subclone(); rA = new Subclone(); rB = new Subclone(); rD = new Subclone();

		p0->setId(10); pA->setId(11); pB->setId(12); pC->setId(13);
		r0->setId(20); rA->setId(21); rB->setId(22); rD->setId(24);



		pA->addEventCluster(&cA); rA->addEventCluster(&cA);
		pB->addEventCluster(&cB); rB->addEventCluster(&cB);
		pC->addEventCluster(&cC); rD->addEventCluster(&cD);

		p0->addChild(pA); pA->addChild(pB); pB->addChild(pC);
		r0->addChild(rA); rA->addChild(rB); rA->addChild(rD);

		p0->setFraction(0.1); pA->setFraction(0.1); pB->setFraction(0.1); pC->setFraction(0.1);
		r0->setFraction(0.1); rA->setFraction(0.1); rB->setFraction(0.1); rD->setFraction(0.1);

		CHECK(TreeMerge(p0, r0));

	}

	// 0, (A, (B, (C))) == 0, (AD, E)
	TEST(test_tree_merge_3_3) {
		CNV A, B, C, D, E;
		A.range.chrom = 1;
		B.range.chrom = 2;
		C.range.chrom = 3;
		D.range.chrom = 4;
		E.range.chrom = 5;

		EventCluster cA, cB, cC, cAD, cE;
		cA.addEvent(&A);
		cB.addEvent(&B);
		cC.addEvent(&C);
		cAD.addEvent(&A);
		cAD.addEvent(&D);
		cE.addEvent(&E);

		Subclone *p0, *pA, *pB, *pC;
		Subclone *r0, *rAD, *rE;

		p0 = new Subclone(); pA = new Subclone(); pB = new Subclone(); pC = new Subclone();
		r0 = new Subclone(); rAD = new Subclone(); rE = new Subclone();

		p0->setId(10); pA->setId(11); pB->setId(12); pC->setId(13);
		r0->setId(20); rAD->setId(214); rE->setId(25);



		pA->addEventCluster(&cA); rAD->addEventCluster(&cAD);
		pB->addEventCluster(&cB); rE->addEventCluster(&cE);
		pC->addEventCluster(&cC);

		p0->addChild(pA); pA->addChild(pB); pB->addChild(pC);
		r0->addChild(rAD); rAD->addChild(rE);

		p0->setFraction(0.1); pA->setFraction(0.1); pB->setFraction(0.1); pC->setFraction(0.1);
		r0->setFraction(0.1); rAD->setFraction(0.1); rE->setFraction(0.1);

		CHECK(TreeMerge(p0, r0));
	}

	// 0, (A, (B, (C))) != 0, (ACD)
	TEST(test_tree_merge_3_4) {
		CNV A, B, C, D;
		A.range.chrom = 1;
		B.range.chrom = 2;
		C.range.chrom = 3;
		D.range.chrom = 4;

		EventCluster cA, cB, cC, cD;
		cA.addEvent(&A);
		cB.addEvent(&B);
		cC.addEvent(&C);
		cD.addEvent(&D);

		Subclone *p0, *pA, *pB, *pC;
		Subclone *r0, *rACD;

		p0 = new Subclone(); pA = new Subclone(); pB = new Subclone(); pC = new Subclone();
		r0 = new Subclone(); rACD = new Subclone();

		p0->setId(10); pA->setId(11); pB->setId(12); pC->setId(13);
		r0->setId(20); rACD->setId(2134);

		pA->addEventCluster(&cA); 
		pB->addEventCluster(&cB); 
		pC->addEventCluster(&cC); 

		rACD->addEventCluster(&cA);
		rACD->addEventCluster(&cC);
		rACD->addEventCluster(&cD);

		p0->addChild(pA); pA->addChild(pB); pB->addChild(pC);
		r0->addChild(rACD);

		p0->setFraction(0.1); pA->setFraction(0.1); pB->setFraction(0.1); pC->setFraction(0.1);
		r0->setFraction(0.1); rACD->setFraction(0.1);

		CHECK(not TreeMerge(p0, r0));

	}

	// aml1
	TEST( test_tree_merge_aml1_1 ) {
		CNV a, b, c, d, e;
		a.range.chrom = 1; b.range.chrom = 2; c.range.chrom = 3; d.range.chrom = 4; e.range.chrom = 5;

		EventCluster cA, cB, cC, cD, cE;
		cA.addEvent(&a); cB.addEvent(&b); cC.addEvent(&c); cD.addEvent(&d), cE.addEvent(&e);

		Subclone *p0, *p1, *p2, *p3, *p4;
		Subclone *r0, *r1, *r3, *r4, *r5;

		p0 = new Subclone(); r0 = new Subclone();
		p1 = new Subclone(); p1->addEventCluster(&cA); r1 = new Subclone(); r1->addEventCluster(&cA);
		p2 = new Subclone(); p2->addEventCluster(&cB);
		p3 = new Subclone(); p3->addEventCluster(&cC); r3 = new Subclone(); r3->addEventCluster(&cC);
		p4 = new Subclone(); p4->addEventCluster(&cD); r4 = new Subclone(); r4->addEventCluster(&cD);
		r5 = new Subclone(); r5->addEventCluster(&cE);

		p0->setId(10); r0->setId(20);
		p1->setId(11); r1->setId(21);
		p2->setId(12);
		p3->setId(13); r3->setId(23);
		p4->setId(14); r4->setId(24);
		r5->setId(25);

		p0->addChild(p1); p1->addChild(p2); p1->addChild(p3); p2->addChild(p4);
		r0->addChild(r1); r1->addChild(r3); r3->addChild(r4); r4->addChild(r5);

		p0->setFraction(0.1); r0->setFraction(0.1);
		p1->setFraction(0.1); r1->setFraction(0.1);
		p2->setFraction(0.1);
		p3->setFraction(0.1); r3->setFraction(0.1);
		p4->setFraction(0.1); r4->setFraction(0.1);
		r5->setFraction(0.1);


		CHECK(not TreeMerge(p0, r0));
	}

	// UPN426980-rel-1
	TEST( test_tree_merge_upn426 ) {
		CNV a, b, c, d, e;
		a.range.chrom = 1; b.range.chrom = 2; c.range.chrom = 3; d.range.chrom = 4; e.range.chrom = 5;

		EventCluster cA, cB, cC, cD, cE;
		cA.addEvent(&a); cB.addEvent(&b); cC.addEvent(&c); cD.addEvent(&d), cE.addEvent(&e);

		Subclone *p0, *p1, *p2;
		Subclone *r0, *r1, *r2, *r3;

		p0 = new Subclone(); r0 = new Subclone();
		p1 = new Subclone(); p1->addEventCluster(&cA); p1->addEventCluster(&cB); p1->addEventCluster(&cC);
		p2 = new Subclone(); p2->addEventCluster(&cD);

		r1 = new Subclone(); r1->addEventCluster(&cA);
		r2 = new Subclone(); r2->addEventCluster(&cB);
		r3 = new Subclone(); r3->addEventCluster(&cE);

		p0->setId(10); r0->setId(20);
		p1->setId(1123); r1->setId(21);
		p2->setId(14); r2->setId(22);
		r3->setId(25);

		p0->addChild(p1); p1->addChild(p2);
		r0->addChild(r1); r1->addChild(r2); r1->addChild(r3);

		p0->setFraction(0); p1->setFraction(0.6); p2->setFraction(0.4);
		r0->setFraction(0); r1->setFraction(0); r2->setFraction(0.3); r3->setFraction(0.7);

		CHECK(TreeMerge(p0, r0));

	}

	// UPN426980-rel-2
	TEST( test_tree_merge_upn426_2 ) {
		CNV a, b, c, d, e;
		a.range.chrom = 1; b.range.chrom = 2; c.range.chrom = 3; d.range.chrom = 4; e.range.chrom = 5;

		EventCluster cA, cB, cC, cD, cE;
		cA.addEvent(&a); cB.addEvent(&b); cC.addEvent(&c); cD.addEvent(&d), cE.addEvent(&e);

		Subclone *p0, *p1, *p2;
		Subclone *r0, *r1, *r2, *r3;

		p0 = new Subclone(); r0 = new Subclone();
		p1 = new Subclone(); p1->addEventCluster(&cA); p1->addEventCluster(&cB); p1->addEventCluster(&cC);
		p2 = new Subclone(); p2->addEventCluster(&cD);

		r1 = new Subclone(); r1->addEventCluster(&cA);
		r2 = new Subclone(); r2->addEventCluster(&cE);
		r3 = new Subclone(); r3->addEventCluster(&cB);

		p0->setId(10); r0->setId(20);
		p1->setId(1123); r1->setId(21);
		p2->setId(14); r2->setId(25);
		r3->setId(22);

		p0->addChild(p1); p1->addChild(p2);
		r0->addChild(r1); r1->addChild(r2); r2->addChild(r3);

		p0->setFraction(0); p1->setFraction(0.6); p2->setFraction(0.4);
		r0->setFraction(0); r1->setFraction(0.3); r2->setFraction(0.45); r3->setFraction(0.25);

		CHECK(not TreeMerge(p0, r0));

	}
}


int main() {
	return UnitTest::RunAllTests();
}
