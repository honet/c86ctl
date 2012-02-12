/***
	c86ctl
	
	Copyright (c) 2009-2012, honet. All rights reserved.
	This software is licensed under the BSD license.

	honet.kk(at)gmail.com
 */
#pragma once

class Chip
{
public:
	Chip(){};
	virtual ~Chip(){};
	
public:
	virtual void reset(){};
	virtual void update(){};

public:
	virtual void filter( int addr, UCHAR *data ){};
	virtual bool setReg( int addr, UCHAR data ){ return false; };
	virtual UCHAR getReg( int addr ){ return 0; };

};

