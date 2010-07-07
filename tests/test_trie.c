/*************************************************************************** 
 *           test_trie.c  - Test suite for trie data structure
 *                  -------------------
 *  begin                : Thu Oct 24 2007
 *  copyright            : (C) 2007 by Patrick Sabin
 *  email                : patricksabin@gmx.at
 ****************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License (revised).                      *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <tests.h>

/*Needs private declarations*/
#include <kdbprivate.h>


Trie *test_insert (Trie *trie, char *name, char* value)
{
	Backend *backend = elektraCalloc (sizeof (Backend));
	backend->mountpoint = keyNew (name, KEY_VALUE, value, KEY_END);
	keyIncRef (backend->mountpoint);
	return elektraTrieInsert(trie, name, backend);
}


void test_minimaltrie()
{
	printf ("Test minimal trie\n");

	Trie *trie = test_insert (0, "", "");
	Key *s = keyNew ("", KEY_END);
	Key *mp = keyNew ("", KEY_VALUE, "", KEY_END);

	succeed_if (elektraTrieLookup (trie, s), "trie should not be null");
	succeed_if (compare_key (elektraTrieLookup (trie, s)->mountpoint, mp)==0, "could not find empty key");

	keySetName (s, "user");
	succeed_if (compare_key (elektraTrieLookup (trie, s)->mountpoint, mp)==0, "could not find empty key");

	keySetName (s, "system");
	succeed_if (compare_key (elektraTrieLookup (trie, s)->mountpoint, mp)==0, "could not find empty key");

	keySetName (s, "user/below");
	succeed_if (compare_key (elektraTrieLookup (trie, s)->mountpoint, mp)==0, "could not find empty key");

	keySetName (s, "system/below");
	succeed_if (compare_key (elektraTrieLookup (trie, s)->mountpoint, mp)==0, "could not find empty key");

	// output_trie (trie);

	elektraTrieClose(trie, 0);
	keyDel (s);
	keyDel (mp);
}

KeySet *simple_config(void)
{
	return ksNew(5,
		keyNew("system/elektra/mountpoints", KEY_END),
		keyNew("system/elektra/mountpoints/simple", KEY_END),
		keyNew("system/elektra/mountpoints/simple/mountpoint", KEY_VALUE, "user/tests/simple", KEY_END),
		KS_END);
}

void test_simple()
{
	printf ("Test simple trie\n");

	Trie *trie = test_insert (0, "user/tests/simple", "simple");

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew("user", KEY_END);
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (!backend, "there should be no backend");


	Key *mp = keyNew("user/tests/simple", KEY_VALUE, "simple", KEY_END);
	keySetName(searchKey, "user/tests/simple");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/tests/simple/below");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/tests/simple/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");

	elektraTrieClose(trie, 0);
	keyDel (mp);
	keyDel (searchKey);
}

void collect_mountpoints(Trie *trie, KeySet *mountpoints)
{
	int i;
	for (i=0; i <= MAX_UCHAR; ++i)
	{
		if (trie->value[i]) ksAppendKey(mountpoints, ((Backend*) trie->value[i])->mountpoint);
		if (trie->children[i]) collect_mountpoints(trie->children[i], mountpoints);
	}
}

void test_iterate()
{
	printf ("Test iterate trie\n");

	Trie *trie = test_insert (0, "user/tests/hosts", "hosts");
	trie = test_insert (trie, "user/tests/hosts/below", "below");

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew("user", KEY_END);
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (!backend, "there should be no backend");


	Key *mp = keyNew("user/tests/hosts", KEY_VALUE, "hosts", KEY_END);
	keySetName(searchKey, "user/tests/hosts");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, mp) == 0, "mountpoint key not correct");
	// printf ("backend: %p\n", (void*)backend);


	keySetName(searchKey, "user/tests/hosts/other/below");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");
	// printf ("b2: %p\n", (void*)b2);


	keySetName(searchKey, "user/tests/hosts/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	Key *mp2 = keyNew("user/tests/hosts/below", KEY_VALUE, "below", KEY_END);
	keySetName(searchKey, "user/tests/hosts/below");
	Backend *b3 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (backend != b3, "should be different backend");
	succeed_if (compare_key(b3->mountpoint, mp2) == 0, "mountpoint key not correct");
	backend = b3;
	// printf ("b3: %p\n", (void*)b3);


	keySetName(searchKey, "user/tests/hosts/below/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (backend == b3, "should be same backend");
	succeed_if (compare_key(b3->mountpoint, mp2) == 0, "mountpoint key not correct");

	// output_trie(trie);

	KeySet *mps = ksNew(0);
	collect_mountpoints(trie, mps);
	succeed_if (ksGetSize (mps) == 2, "not both mountpoints collected");
	succeed_if (compare_key(ksHead(mps), mp) == 0, "not correct mountpoint found");
	succeed_if (compare_key(ksTail(mps), mp2) == 0, "not correct mountpoint found");
	ksDel (mps);

	elektraTrieClose(trie, 0);

	keyDel (mp);
	keyDel (mp2);
	keyDel (searchKey);
}

void test_reviterate()
{
	printf ("Test reviterate trie\n");

	Trie * trie = test_insert (0, "user/tests/hosts/below", "below");
	trie = test_insert (trie, "user/tests/hosts", "hosts");

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew("user", KEY_END);
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (!backend, "there should be no backend");


	Key *mp = keyNew("user/tests/hosts", KEY_VALUE, "hosts", KEY_END);
	keySetName(searchKey, "user/tests/hosts");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, mp) == 0, "mountpoint key not correct");
	// printf ("backend: %p\n", (void*)backend);


	keySetName(searchKey, "user/tests/hosts/other/below");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");
	// printf ("b2: %p\n", (void*)b2);


	keySetName(searchKey, "user/tests/hosts/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	Key *mp2 = keyNew("user/tests/hosts/below", KEY_VALUE, "below", KEY_END);
	keySetName(searchKey, "user/tests/hosts/below");
	Backend *b3 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (backend != b3, "should be different backend");
	succeed_if (compare_key(b3->mountpoint, mp2) == 0, "mountpoint key not correct");
	backend = b3;
	// printf ("b3: %p\n", (void*)b3);


	keySetName(searchKey, "user/tests/hosts/below/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (backend == b3, "should be same backend");
	succeed_if (compare_key(b3->mountpoint, mp2) == 0, "mountpoint key not correct");

	// output_trie(trie);

	KeySet *mps = ksNew(0);
	collect_mountpoints(trie, mps);
	succeed_if (ksGetSize (mps) == 2, "not both mountpoints collected");
	succeed_if (compare_key(ksHead(mps), mp) == 0, "not correct mountpoint found");
	succeed_if (compare_key(ksTail(mps), mp2) == 0, "not correct mountpoint found");
	ksDel (mps);

	elektraTrieClose(trie, 0);

	keyDel (mp);
	keyDel (mp2);
	keyDel (searchKey);
}

KeySet *moreiterate_config(void)
{
	return ksNew(50,
		keyNew("system/elektra/mountpoints", KEY_END),
		keyNew("system/elektra/mountpoints/user", KEY_END),
		keyNew("system/elektra/mountpoints/user/mountpoint", KEY_VALUE, "user", KEY_END),
		keyNew("system/elektra/mountpoints/tests", KEY_END),
		keyNew("system/elektra/mountpoints/tests/mountpoint", KEY_VALUE, "user/tests", KEY_END),
		keyNew("system/elektra/mountpoints/hosts", KEY_END),
		keyNew("system/elektra/mountpoints/hosts/mountpoint", KEY_VALUE, "user/tests/hosts", KEY_END),
		keyNew("system/elektra/mountpoints/below", KEY_END),
		keyNew("system/elektra/mountpoints/below/mountpoint", KEY_VALUE, "user/tests/hosts/below", KEY_END),
		keyNew("system/elektra/mountpoints/system", KEY_END),
		keyNew("system/elektra/mountpoints/system/mountpoint", KEY_VALUE, "system", KEY_END),
		keyNew("system/elektra/mountpoints/systests", KEY_END),
		keyNew("system/elektra/mountpoints/systests/mountpoint", KEY_VALUE, "system/tests", KEY_END),
		keyNew("system/elektra/mountpoints/syshosts", KEY_END),
		keyNew("system/elektra/mountpoints/syshosts/mountpoint", KEY_VALUE, "system/tests/hosts", KEY_END),
		keyNew("system/elektra/mountpoints/sysbelow", KEY_END),
		keyNew("system/elektra/mountpoints/sysbelow/mountpoint", KEY_VALUE, "system/tests/hosts/below", KEY_END),
		KS_END);
}

KeySet *set_mountpoints(void)
{
	return ksNew(10,
		keyNew("user", KEY_VALUE, "user", KEY_END),
		keyNew("user/tests", KEY_VALUE, "tests", KEY_END),
		keyNew("user/tests/hosts", KEY_VALUE, "hosts", KEY_END),
		keyNew("user/tests/hosts/below", KEY_VALUE, "below", KEY_END),
		keyNew("system", KEY_VALUE, "system", KEY_END),
		keyNew("system/tests", KEY_VALUE, "systests", KEY_END),
		keyNew("system/tests/hosts", KEY_VALUE, "syshosts", KEY_END),
		keyNew("system/tests/hosts/below", KEY_VALUE, "sysbelow", KEY_END),
		KS_END);
}

void test_moreiterate()
{
	printf ("Test moreiterate trie\n");

	Trie *trie = test_insert (0, "user", "user");
	trie = test_insert (trie, "user/tests", "tests");
	trie = test_insert (trie, "user/tests/hosts", "hosts");
	trie = test_insert (trie, "user/tests/hosts/below", "below");
	trie = test_insert (trie, "system", "system");
	trie = test_insert (trie, "system/tests", "systests");
	trie = test_insert (trie, "system/tests/hosts", "syshosts");
	trie = test_insert (trie, "system/tests/hosts/below", "sysbelow");

	KeySet *mps = set_mountpoints();

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew(0);

	keySetName(searchKey, "user");
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, ksLookupByName(mps, "user",0)) == 0, "mountpoint key not correct");
	// printf ("backend: %p\n", (void*)backend);


	keySetName(searchKey, "user/tests/hosts/other/below");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "user/tests/hosts",0)) == 0, "mountpoint key not correct");
	// printf ("b2: %p\n", (void*)b2);


	keySetName(searchKey, "user/tests/hosts/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "user/tests/hosts",0)) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/tests/hosts/below");
	Backend *b3 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "user/tests/hosts/below",0)) == 0, "mountpoint key not correct");
	backend = b3;
	// printf ("b3: %p\n", (void*)b3);


	keySetName(searchKey, "user/tests/hosts/below/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "user/tests/hosts/below",0)) == 0, "mountpoint key not correct");

	keySetName(searchKey, "system");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, ksLookupByName(mps, "system",0)) == 0, "mountpoint key not correct");
	// printf ("backend: %p\n", (void*)backend);


	keySetName(searchKey, "system/tests/hosts/other/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "system/tests/hosts",0)) == 0, "mountpoint key not correct");
	// printf ("b2: %p\n", (void*)b2);


	keySetName(searchKey, "system/tests/hosts/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "system/tests/hosts",0)) == 0, "mountpoint key not correct");


	keySetName(searchKey, "system/tests/hosts/below");
	b3 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "system/tests/hosts/below",0)) == 0, "mountpoint key not correct");
	backend = b3;
	// printf ("b3: %p\n", (void*)b3);


	keySetName(searchKey, "system/tests/hosts/below/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "system/tests/hosts/below",0)) == 0, "mountpoint key not correct");

	// output_trie(trie);

	KeySet *mps_cmp = ksNew(0);
	collect_mountpoints(trie, mps_cmp);
	succeed_if (ksGetSize(mps_cmp) == 8, "size should be 8");
	succeed_if (compare_keyset(mps, mps_cmp) == 0, "could not collect mountpoints");

	ksDel (mps_cmp);
	ksDel (mps);

	elektraTrieClose(trie, 0);
	keyDel (searchKey);
}

void test_revmoreiterate()
{
	printf ("Test revmoreiterate trie\n");

	for (int i=0; i<5; ++i)
	{

	Trie *trie = 0;
	switch (i)
	{
	case 0:
		trie = test_insert (trie, "user/tests", "tests");
		trie = test_insert (trie, "user/tests/hosts", "hosts");
		trie = test_insert (trie, "user/tests/hosts/below", "below");
		trie = test_insert (trie, "system/tests", "systests");
		trie = test_insert (trie, "system/tests/hosts", "syshosts");
		trie = test_insert (trie, "system/tests/hosts/below", "sysbelow");
		trie = test_insert (trie, "system", "system");
		trie = test_insert (trie, "user", "user");
		break;
	case 1:
		trie = test_insert (trie, "system/tests/hosts", "syshosts");
		trie = test_insert (trie, "system", "system");
		trie = test_insert (trie, "user/tests", "tests");
		trie = test_insert (trie, "user/tests/hosts", "hosts");
		trie = test_insert (trie, "user/tests/hosts/below", "below");
		trie = test_insert (trie, "system/tests", "systests");
		trie = test_insert (trie, "user", "user");
		trie = test_insert (trie, "system/tests/hosts/below", "sysbelow");
		break;
	case 2:
		trie = test_insert (trie, "system/tests/hosts/below", "sysbelow");
		trie = test_insert (trie, "system/tests/hosts", "syshosts");
		trie = test_insert (trie, "user/tests/hosts/below", "below");
		trie = test_insert (trie, "user/tests/hosts", "hosts");
		trie = test_insert (trie, "user/tests", "tests");
		trie = test_insert (trie, "user", "user");
		trie = test_insert (trie, "system/tests", "systests");
		trie = test_insert (trie, "system", "system");
		break;
	case 3:
		trie = test_insert (trie, "user/tests/hosts/below", "below");
		trie = test_insert (trie, "user/tests/hosts", "hosts");
		trie = test_insert (trie, "user/tests", "tests");
		trie = test_insert (trie, "user", "user");
		trie = test_insert (trie, "system/tests/hosts/below", "sysbelow");
		trie = test_insert (trie, "system/tests/hosts", "syshosts");
		trie = test_insert (trie, "system/tests", "systests");
		trie = test_insert (trie, "system", "system");
		break;
	case 4:
		trie = test_insert (trie, "system/tests/hosts/below", "sysbelow");
		trie = test_insert (trie, "system/tests/hosts", "syshosts");
		trie = test_insert (trie, "system/tests", "systests");
		trie = test_insert (trie, "system", "system");
		trie = test_insert (trie, "user/tests/hosts/below", "below");
		trie = test_insert (trie, "user/tests/hosts", "hosts");
		trie = test_insert (trie, "user/tests", "tests");
		trie = test_insert (trie, "user", "user");
		break;
	}

	KeySet *mps = set_mountpoints();

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew(0);

	keySetName(searchKey, "user");
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, ksLookupByName(mps, "user",0)) == 0, "mountpoint key not correct");
	// printf ("backend: %p\n", (void*)backend);


	keySetName(searchKey, "user/tests/hosts/other/below");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "user/tests/hosts",0)) == 0, "mountpoint key not correct");
	// printf ("b2: %p\n", (void*)b2);


	keySetName(searchKey, "user/tests/hosts/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "user/tests/hosts",0)) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/tests/hosts/below");
	Backend *b3 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "user/tests/hosts/below",0)) == 0, "mountpoint key not correct");
	backend = b3;
	// printf ("b3: %p\n", (void*)b3);


	keySetName(searchKey, "user/tests/hosts/below/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "user/tests/hosts/below",0)) == 0, "mountpoint key not correct");

	keySetName(searchKey, "system");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, ksLookupByName(mps, "system",0)) == 0, "mountpoint key not correct");
	// printf ("backend: %p\n", (void*)backend);


	keySetName(searchKey, "system/tests/hosts/other/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "system/tests/hosts",0)) == 0, "mountpoint key not correct");
	// printf ("b2: %p\n", (void*)b2);


	keySetName(searchKey, "system/tests/hosts/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (compare_key(b2->mountpoint, ksLookupByName(mps, "system/tests/hosts",0)) == 0, "mountpoint key not correct");


	keySetName(searchKey, "system/tests/hosts/below");
	b3 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "system/tests/hosts/below",0)) == 0, "mountpoint key not correct");
	backend = b3;
	// printf ("b3: %p\n", (void*)b3);


	keySetName(searchKey, "system/tests/hosts/below/other/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b3, "there should be a backend");
	succeed_if (compare_key(b3->mountpoint, ksLookupByName(mps, "system/tests/hosts/below",0)) == 0, "mountpoint key not correct");

	/*
	printf ("---------\n");
	output_trie(trie);
	*/

	KeySet *mps_cmp = ksNew(0);
	collect_mountpoints(trie, mps_cmp);
	succeed_if (ksGetSize(mps_cmp) == 8, "size should be 8");
	succeed_if (compare_keyset(mps, mps_cmp) == 0, "could not collect mountpoints");

	ksDel (mps_cmp);
	ksDel (mps);

	elektraTrieClose(trie, 0);
	keyDel (searchKey);

	} // end for
}


void test_umlauts()
{
	printf ("Test umlauts trie\n");

	Trie *trie = test_insert (0, "user/umlauts/test", "slash");
	trie = test_insert (trie, "user/umlauts#test", "hash");
	trie = test_insert (trie, "user/umlauts test", "space");
	trie = test_insert (trie, "user/umlauts\200test", "umlauts");

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew("user", KEY_END);
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (!backend, "there should be no backend");


	Key *mp = keyNew("user/umlauts/test", KEY_VALUE, "slash", KEY_END);
	keySetName(searchKey, "user/umlauts/test");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/umlauts#test");
	keySetName(mp, "user/umlauts#test");
	keySetString(mp, "hash");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend != b2, "should be other backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/umlauts test");
	keySetName(mp, "user/umlauts test");
	keySetString(mp, "space");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend != b2, "should be other backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");

	keySetName(searchKey, "user/umlauts\200test");
	keySetName(mp, "user/umlauts\200test");
	keySetString(mp, "umlauts");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend != b2, "should be other backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");

	// output_trie(trie);

	elektraTrieClose(trie, 0);
	keyDel (mp);
	keyDel (searchKey);
}

#if 0

KeySet *endings_config(void)
{
	return ksNew(5,
		keyNew("system/elektra/mountpoints", KEY_END),
		keyNew("system/elektra/mountpoints/slash", KEY_END),
		keyNew("system/elektra/mountpoints/slash/mountpoint", KEY_VALUE, "user/endings", KEY_END),
		keyNew("system/elektra/mountpoints/hash", KEY_END),
		keyNew("system/elektra/mountpoints/hash/mountpoint", KEY_VALUE, "user/endings#", KEY_END),
		keyNew("system/elektra/mountpoints/space", KEY_END),
		keyNew("system/elektra/mountpoints/space/mountpoint", KEY_VALUE, "user/endings ", KEY_END),
		keyNew("system/elektra/mountpoints/endings", KEY_END),
		keyNew("system/elektra/mountpoints/endings/mountpoint", KEY_VALUE, "user/endings\200", KEY_END),
		KS_END);
}

void test_endings()
{
	printf ("Test endings trie\n");

	Key *errorKey = keyNew(0);
	KeySet *modules = modules_config();
	Trie *trie = elektraTrieOpen(endings_config(), modules, errorKey);

	output_warnings (errorKey);
	output_errors (errorKey);

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew("user");
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (!backend, "there should be no backend");


	Key *mp = keyNew("user/endings", KEY_VALUE, "slash", KEY_END);
	keySetName(searchKey, "user/endings");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/endings#");
	keySetName(mp, "user/endings#");
	keySetString(mp, "hash");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend != b2, "should be other backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/endings/_");
	keySetName(mp, "user/endings");
	keySetString(mp, "slash");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be the same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/endings/X");
	keySetName(mp, "user/endings");
	keySetString(mp, "slash");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be the same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/endings_");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (!b2, "there should be no backend");


	keySetName(searchKey, "user/endingsX");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (!b2, "there should be no backend");


	keySetName(searchKey, "user/endings!");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (!b2, "there should be no backend");


	keySetName(searchKey, "user/endings ");
	keySetName(mp, "user/endings ");
	keySetString(mp, "space");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend != b2, "should be other backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");

	keySetName(searchKey, "user/endings\200");
	keySetName(mp, "user/endings\200");
	keySetString(mp, "endings");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend != b2, "should be other backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");

	// output_trie(trie);

	elektraTrieClose(trie, 0);
	keyDel (errorKey);
	ksDel (modules);
	keyDel (mp);
	keyDel (searchKey);
}

KeySet *root_config(void)
{
	return ksNew(5,
		keyNew("system/elektra/mountpoints", KEY_END),
		keyNew("system/elektra/mountpoints/root", KEY_END),
		keyNew("system/elektra/mountpoints/root/mountpoint", KEY_VALUE, "", KEY_END),
		keyNew("system/elektra/mountpoints/simple", KEY_END),
		keyNew("system/elektra/mountpoints/simple/mountpoint", KEY_VALUE, "user/tests/simple", KEY_END),
		KS_END);
}

void test_root()
{
	printf ("Test trie with root\n");

	Key *errorKey = keyNew(0);
	KeySet *modules = modules_config();
	Trie *trie = elektraTrieOpen(root_config(), modules, errorKey);

	output_warnings (errorKey);
	output_errors (errorKey);

	exit_if_fail (trie, "trie was not build up successfully");

	Key *searchKey = keyNew("user");
	Key *rmp = keyNew("", KEY_VALUE, "root", KEY_END);
	Backend *backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be the root backend");
	succeed_if (compare_key(backend->mountpoint, rmp) == 0, "mountpoint key not correct");


	Key *mp = keyNew("user/tests/simple", KEY_VALUE, "simple", KEY_END);
	keySetName(searchKey, "user/tests/simple");
	backend = elektraTrieLookup(trie, searchKey);
	succeed_if (backend, "there should be a backend");
	succeed_if (compare_key(backend->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/tests/simple/below");
	Backend *b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");


	keySetName(searchKey, "user/tests/simple/deep/below");
	b2 = elektraTrieLookup(trie, searchKey);
	succeed_if (b2, "there should be a backend");
	succeed_if (backend == b2, "should be same backend");
	succeed_if (compare_key(b2->mountpoint, mp) == 0, "mountpoint key not correct");

	// output_trie(trie);

	elektraTrieClose(trie, 0);
	keyDel (errorKey);
	ksDel (modules);
	keyDel (mp);
	keyDel (rmp);
	keyDel (searchKey);
}

#endif

int main(int argc, char** argv)
{
	printf("TRIE       TESTS\n");
	printf("==================\n\n");

	init (argc, argv);

	test_minimaltrie();
	test_simple();
	test_iterate();
	test_reviterate();
	test_moreiterate();
	test_revmoreiterate();
	test_umlauts();
	/*
	test_endings();
	test_root();
	test_rootsimple();
	test_realworld();
	*/

	printf("\ntest_trie RESULTS: %d test(s) done. %d error(s).\n", nbTest, nbError);

	return nbError;
}

