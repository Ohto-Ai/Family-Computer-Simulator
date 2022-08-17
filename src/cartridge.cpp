﻿#include "total.h"
#include <string>
#include <fstream>
#include <QDebug>
#include <string.h>
#include <QFileInfo>

using namespace std;

void Cartridge::read_from_file(string input_file)
{
	//1.
	int fsize = QFileInfo("tank.nes").size();
	uint8_t* nes_data = new uint8_t[fsize];
	ifstream stream(input_file, ios::in | ios::binary);
	if (!stream) {
		qDebug() << "Read NES File Error!" << "\n";
		abort();
	}
	stream.read((char*)nes_data, fsize);
	stream.close();
	if (nes_data[0] != 'N' || nes_data[1] != 'E' || nes_data[2] != 'S' || nes_data[3] != '\x1A') {
		qDebug() << "First 4 bytes in file must be NES\\x1A!" << "\n";
		abort();
	}
	//2.
	rom_num = nes_data[4];
	vrom_num = nes_data[5];
	uint8_t nametable_mirror = nes_data[6] & 0xb;//
	uint8_t mapper_type = ((nes_data[6] >> 4) & 0xf) | (nes_data[7] & 0xf0);
	has_added_ram = bool(nes_data[6] & 0x2);
	//3.
	if (mapper_type == 0) {
		mapper_ptr = new Mapper0();
		mapper_ptr->nametable_mirror = nametable_mirror;
	}
	else if (mapper_type == 1) {
		mapper_ptr = new Mapper1();
		mapper_ptr->nametable_mirror = nametable_mirror;
	}
	else if (mapper_type == 2) {
		mapper_ptr = new Mapper2();
		mapper_ptr->nametable_mirror = nametable_mirror;
	}
	else {
		qDebug() << "Error: Unsupported Mapper = " << mapper_type << "\n";
		abort();
	}
	//4.
	uint32_t rom_start_dx = 16;
	uint32_t vrom_start_dx = rom_num * 16384 + 16;
	program_data = new uint8_t[16384 * rom_num];
	memcpy(program_data, &nes_data[rom_start_dx], 16384 * rom_num);
	vrom_data = new uint8_t[8192 * vrom_num];
	memcpy(vrom_data, &nes_data[vrom_start_dx], 8192 * vrom_num);
}