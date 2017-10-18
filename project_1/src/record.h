/*
 * record.h
 *
 *  Created on: Oct 9, 2016
 *      Author: vangelis
 */

#ifndef RECORD_H_
#define RECORD_H_

typedef struct Record {
	int studid;
	char *lastname;
	char *firstname;
	float gpa;
	int numcourses;
	char *deprt;
	int postcode;
} Record;


#endif /* RECORD_H_ */
