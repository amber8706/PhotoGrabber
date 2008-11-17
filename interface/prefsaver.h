/*
****************************************************************
* Copyright (c) 2004-2005,	Jan-Rixt Van Hoye				   *
* All rights reserved.										   *
* Distributed under the terms of the MIT License.              *
****************************************************************
*/
//
//		File defenition

#ifndef PREF_SAVER_H
#define PREF_SAVER_H

//
//		Includes
#include <FindDirectory.h>
#include <Path.h>
#include <File.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Entry.h>

//
//		Preference Saver Class
class BeCam_PrefSaver
{
	public:
		BeCam_PrefSaver(const char *name,const int no_of_fields);
		~BeCam_PrefSaver();
		
		void AddField(const char *f,const char *entry);
		
		bool GetString(const char *f,char *entry,const unsigned short buf_size);
		
		bool SaveFile();
		bool GetLine(BEntry *the_file,long line_number,char *buffer); 
			//useful spin-off function that grabs a line of a file (by counting '\n' s)
			//Its only to be used for small files though, ie: less than 1mb.
			//It'll get really slow otherwise.
			//GetLine() returns false if sucessful.
			
	private:		
		char 	**field ;//Arrays containing of lines in text form
		char 	**values;//each one is of this format:
						 // "field = value\n"
						 //The field is user-defined (can be any string)
						 //value is a string, which might be a 'string-isted' number
	
		int Get_Position(const char *f); //checks for existing Field
		
	protected: 
		int max_no_of_lines; //used to keep check
		int current_number; //stores the number of fields already set	
		BEntry *file;	//This is the magic file that saves all the stuff
};

#endif

		
		
