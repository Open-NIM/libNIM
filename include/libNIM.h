/*
 * libNIM a cross platform library for the Open-NIM modules.
 * Copyright (C) 2022 Alluringbits
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * THERE IS NO WARRANTY FOR THE PROGRAM, TO THE EXTENT PERMITTED BY
 * APPLICABLE LAW.  EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT
 * HOLDERS AND/OR OTHER PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE PROGRAM
 * IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF
 * ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 */
#pragma once
#include "serial.h"
#include <cstdint>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <new>

namespace NIM{
	enum moduleType {counter, universal, unknown};
	struct Module_base{
			Module_base(const NIM::moduleType t, const std::string pi, uint64_t srl);
			Module_base(NIM::Module_base &&m) = default;
			Module_base & operator=(Module_base &&m) = default;
			~Module_base(){};
			

			constexpr moduleType getType() const noexcept {return tp;};
			constexpr uint64_t getSerialNumber() const noexcept {return serialNumber;};
		protected:
			moduleType tp;
			serial::Serial sp{};
			const uint64_t serialNumber;

			std::string send_and_check(const std::string &s, const uint8_t size, uint16_t N = 5 );
			/* static constexpr std::string_view srlnstr{"SRLN"}; */
			/* static constexpr std::string_view rstastr{"RSTA"}; */
			static constexpr std::string_view errstr{"ERR!"};
		private:
			Module_base(const NIM::Module_base &m) = delete;
			Module_base operator=(const Module_base &m) = delete;
			
	};
	using Module_ptr=std::shared_ptr<NIM::Module_base>;
	using Module=Module_base;

	struct Counter : public Module_base{
			Counter(std::string pi, uint64_t srln) : Module_base{counter, pi, srln}{};
			
			enum command_noargs_bool {RSTA, RST1, RST2, EN_1, EN_2};
			enum command_noargs_32ret {GET1 = 6, GET2};
			enum command_noargs_64ret {SRLN = 5};
			//enum command_args_bool {SET1, SET2};

			bool send(const  NIM::Counter::command_noargs_bool cmd) ;
			//bool send(const NIM::Counter::command_args_bool cmd, const std::vector<uint8_t> arg) const;
			uint32_t send(const  NIM::Counter::command_noargs_32ret cmd) ; 
			uint64_t send(const  NIM::Counter::command_noargs_64ret cmd) ;
		private:
			const std::string cmdList[8]{{'R','S','T','A'},{'R','S','T','1'},{'R','S','T','2'},{'E','N','_','1'},{'E','N','_','2'},{'S','R','L','N'},{'G','E','T','1'},{'G','E','T','2'}};
	};

	

	std::vector<std::shared_ptr<NIM::Module_base>> listAvailableModules();
	uint64_t send_serialNumber_request(serial::Serial &sp, uint16_t N= 5);
	uint32_t string_to_uint32_t(const std::string &s);
	uint64_t string_to_uint64_t(const std::string &s);
	std::string typeStr(moduleType t);

	//Add error classes for custom errors and wrappers around the serial errors
	
	class ModuleUnreachable : public std::exception{
			std::string desc;
		public:
			ModuleUnreachable(const std::string port) : desc{std::string("A possible module on port [") + port + "] was found, but the port is unreachable and cannot be opened."}{};
			ModuleUnreachable(const std::string& port, moduleType t, uint64_t sn, const std::string & cmd) : desc{std::string("An error occurred while trying to reach  the Module of type \"")+typeStr(t)+"\" with Serial Number (" + std::to_string(sn) + ") on port ["+ port + "], while trying to send the command \""+cmd+"\"."}{};
			const ModuleUnreachable& operator=(const ModuleUnreachable & a) = delete;
			~ModuleUnreachable(){};
			const char * what() const noexcept override{return desc.c_str();};
	};

	class MaxCommunicationAttempts : public std::exception{
			std::string desc;
		public:
			MaxCommunicationAttempts(const std::string& port) : desc{std::string("The maximum number of attempts was reached when trying to connect to a possible module on port [")+ port + "]."}{};
			MaxCommunicationAttempts(const std::string& port, moduleType t, uint64_t sn) : desc{std::string("The maximum number of attempts was reached when trying to communicate with the Module of type \"")+typeStr(t)+"\" with Serial Number (" + std::to_string(sn) + ") on port ["+ port + "]."}{};
			MaxCommunicationAttempts(const std::string& port, moduleType t, uint64_t sn, const std::string & cmd) : desc{std::string("The maximum number of attempts was reached when trying to send the command \""+cmd+"\" to the Module of type \"")+typeStr(t)+"\" with Serial Number (" + std::to_string(sn) + ") on port ["+ port + "]."}{};
			const MaxCommunicationAttempts& operator=(const MaxCommunicationAttempts & a) = delete;

			~MaxCommunicationAttempts(){};
			const char * what() const noexcept override{return desc.c_str();};
	};

	class UnexpectedException : public std::exception{
			std::string desc;
		public:
			UnexpectedException(const std::exception &e) : desc{std::string("An Unexpected error has occurred. Please communicate this problem to the developers of libNIM by opening an issue on the official github repository at [https://github.com/Open-NIM/libNIM] or contact us at [support@opennim.com].\nError information (what()):\n\t")+e.what()}{};
			const UnexpectedException& operator=(const UnexpectedException & a) = delete;
			~UnexpectedException(){};
			const char * what() const noexcept override{return desc.c_str();};
	};

	//PROTOTYPE POSSIBLE IMPLEMENTATIONS OF OTHER EXCEPTIONS
	/* class TimeoutException : public std::exception{ */
	/* 		std::string desc; */
	/* 	public: */
	/* 		TimeoutException(const std::string& port, moduleType t, uint64_t sn, const std::string & cmd) : desc{std::string("The timeout was reached when attempting to send the command \"")+cmd+"\" to the Module of type \""+typeStr(t)+"\" with Serial Number (" + std::to_string(sn) + ") on port ["+ port + "]."}{}; */
	/* 		const TimeoutException& operator=(const TimeoutException & a) = delete; */
	/* 		~TimeoutException(){}; */
	/* 		const char * what() const noexcept override{return desc.c_str();}; */
	/* }; */
	
	/* class SerialNumberMismatch : public std::exception{ */
	/* 		std::string desc; */
	/* 	public: */
	/* 		SerialNumberMismatch(const std::string & port, const uint64_t a, const uint64_t b) : desc{std::string("There has been a mismatch between the provided serial number (")+ std::to_string(a) + ") and the serial number that was found ("+std::to_string(b)+") on the module on port ["+port+"]."}{}; */
	/* 		const SerialNumberMismatch& operator=(const SerialNumberMismatch & a) = delete; */
	/* 		~SerialNumberMismatch(){}; */
	/* 		const char * what() const noexcept override{return desc.c_str();}; */

	/* }; */
}



