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

namespace NIM{
	//Remember to use PNG::messages-like library for function return calls/throws if it turns out useful compared to return integers.
	enum moduleType {counter, universal};
	struct Module{
			Module(NIM::moduleType t, std::string &pi, uint64_t srl) :  tp{t}, sp{pi}, serialNumber{srl}{};
			Module(NIM::Module &&m) = default;
			Module& operator=(Module &&m) = default;
			~Module(){};
			

			constexpr moduleType getType() const noexcept {return tp;};
			constexpr uint64_t getSerialNumber() const noexcept {return serialNumber;};
		protected:
			/* enum class universalCmd_noargs { SRLN, RSTA}; */
			/* enum class universalCmd_args { }; */
			/* void send_universal(const universalCmd_noargs cmd) const; */
			/* void send_universal(const universalCmd_args cmd, const std::vector<uint8_t> arg) const; */

			moduleType tp;
			serial::Serial sp;
			const uint64_t serialNumber;

			std::string send_and_check(const std::string &s);
			/* static constexpr std::string_view srlnstr{"SRLN"}; */
			/* static constexpr std::string_view rstastr{"RSTA"}; */
			static constexpr std::string_view errstr{"ERR!"};
		private:
			Module(const NIM::Module &m) = delete;
			Module operator=(const Module &m) = delete;
			
	};

	struct Counter : public Module{
			Counter(std::string &pi, uint64_t srln) : Module{counter, pi, srln}{};
			
			enum command_noargs_bool {RSTA, RST1, RST2, EN_1, EN_2};
			enum command_noargs_32ret {GET1 = 6, GET2};
			enum command_noargs_64ret {SRLN = 5};
			//enum command_args_bool {SET1, SET2};

			bool send(const  NIM::Counter::command_noargs_bool cmd) ;
			//bool send(const NIM::Counter::command_args_bool cmd, const std::vector<uint8_t> arg) const;
			uint32_t send(const  NIM::Counter::command_noargs_32ret cmd) ; 
			uint64_t send(const  NIM::Counter::command_noargs_64ret cmd) ;
		private:
			const std::string cmdList[8]{{'R','S','T','A'},{'R','S','T','1'},{'R','S','T','2'},{'E','N','_','1'},{'E','N','_','2'},{'G','E','T','1'},{'G','E','T','2'}};
	};

	

	std::vector<std::unique_ptr<NIM::Module>> listAvailableModules();
	uint64_t send_serialNumber_request(serial::Serial &sp);
	uint32_t string_to_uint32_t(const std::string &s);
	uint64_t string_to_uint64_t(const std::string &s);


}



