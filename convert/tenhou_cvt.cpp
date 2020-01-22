#include "tinyxml2.h"

#include <fstream>
#include <locale>
#include <cstring>
#include <cstdlib>
#include <string>

using namespace std;
using namespace tinyxml2;

const char* ROUND[12] = { "East 1","East 2","East 3","East 4","South 1","South 2","South 3","South 4","West 1","West 2","West 3","West 4" };
const char* TILE[34] = { "1m","2m","3m","4m","5m","6m","7m","8m","9m","1p","2p","3p","4p","5p","6p","7p","8p","9p","1s","2s","3s","4s","5s","6s","7s","8s","9s","E","S","W","N","Wh","G","R" };
const char* TILE0[34] = { "1m","2m","3m","4m","5ma","6m","7m","8m","9m","1p","2p","3p","4p","5pa","6p","7p","8p","9p","1s","2s","3s","4s","5sa","6s","7s","8s","9s","E","S","W","N","Wh","G","R" };
const char* TYPE[12][2] = { {u8"対コンピュータ戦",""},{u8"赤"},{u8"喰"},{u8"東", u8"南"},{u8"四", u8"三"},{},{u8"速", ""},{},{"", u8"暗"},{"",u8"祝"} ,{"", u8"雀荘"},{"",u8"技能"} };
const char* GAME_NAME[4] = { u8"般",u8"上",u8"特",u8"鳳" };
const char* SEAT[4] = { "East","South","West","North" };
int CHII[21] = { 0,1,2,3,4,5,6,9,10,11,12,13,14,15,18,19,20,21,22,23,24 };
const char* RANK[7] = { "", u8"満貫",u8"跳満",u8"倍満",u8"三倍満",u8"数え役満" };
const char* RANK_Y[5] = { "", u8"役満",u8"二倍役満",u8"三倍役満",u8"四倍役満" };
const char* YAKU[55] = { u8"門前清自摸和",u8"立直",u8"一発",u8"槍槓",u8"嶺上開花",
	u8"海底摸月",u8"河底撈魚",u8"平和",u8"断幺九",u8"一盃口",
	u8"自風 東",u8"自風 南",u8"自風 西",u8"自風 北",
	u8"場風 東",u8"場風 南",u8"場風 西",u8"場風 北",
	u8"役牌 白",u8"役牌 發",u8"役牌 中",
	u8"両立直",u8"七対子",u8"混全帯幺九",u8"一気通貫",u8"三色同順",
	u8"三色同刻",u8"三槓子",u8"対々和",u8"三暗刻",u8"小三元",u8"混老頭",
	u8"二盃口",u8"純全帯幺九",u8"混一色",
	u8"清一色",u8"人和",
	u8"天和",u8"地和",u8"大三元",u8"四暗刻",u8"四暗刻単騎",u8"字一色",
	u8"緑一色",u8"清老頭",u8"九蓮宝燈",u8"純正九蓮宝燈",u8"国士無双",
	u8"国士無双１３面",u8"大四喜",u8"小四喜",u8"四槓子",
	u8"ドラ",u8"裏ドラ",u8"赤ドラ" };

XMLDocument doc;
fstream fout;

XMLElement* mjlog = nullptr;
const XMLAttribute* ver = nullptr;

XMLElement* shuffle = nullptr;
const char* seed = nullptr;
const char* shuffle_ref = nullptr;

XMLElement* gametype = nullptr;
int type = 0;
int lobby = 0;
int player_num = 0;
string game_name;

XMLElement* names = nullptr;
const char* name_[4] = { nullptr };
string name[4];

XMLElement* game = nullptr;
XMLElement* cur_element = nullptr;
const char* temp_att = nullptr;
int cur_round, honba, kyotaku, dora, oya;
bool new_dora = false;
int score[4] = { 0 };
int hands[4][13] = {};
int plycnt = 0, prev_tile = -1;

void game_info(void)
{
	int j;
	char ori_char, trans_char;
	mjlog = doc.RootElement();
	ver = mjlog->FindAttribute("ver");
	shuffle = mjlog->FirstChildElement("SHUFFLE");
	seed = shuffle->Attribute("seed");
	shuffle_ref = shuffle->Attribute("ref");
	gametype = mjlog->FirstChildElement("GO");
	type = gametype->FindAttribute("type")->IntValue();
	player_num = 4 - ((type & 0x0010) >> 4);
	game_name = TYPE[4][(type & 0x0010) >> 4];
	game_name += GAME_NAME[(type & 0x0020) >> 4 | (type & 0x0080) >> 7];
	for (size_t i = 3; i > 0; i--)
	{
		game_name += TYPE[i][(type & (1 << i)) >> i];
	}
	lobby = gametype->IntAttribute("lobby");
	names = mjlog->FirstChildElement("UN");
	name_[0] = names->Attribute("n0");
	name_[1] = names->Attribute("n1");
	name_[2] = names->Attribute("n2");
	if (player_num == 4)
	{
		name_[3] = names->Attribute("n3");
	}
	for (size_t i = 0; i < player_num; i++)
	{
		j = 0;
		while ((ori_char = *(name_[i] + j)) != '\0')
		{
			trans_char = '\0';
			ori_char = *(name_[i] + (++j));
			trans_char += (ori_char >= 'A') ? (ori_char - 'A' + 10) : (ori_char - '0');
			trans_char *= 16;
			ori_char = *(name_[i] + (++j));
			trans_char += (ori_char >= 'A') ? (ori_char - 'A' + 10) : (ori_char - '0');
			name[i] += trans_char;
			j++;
		}
	}
}

void print_tags()
{
	fout << '[' << "Event " << '\"' << game_name << '\"' << "]" << endl;
	fout << '[' << "Site " << '\"' << u8"天鳳 lobby:" << lobby << '\"' << "]" << endl;
	fout << '[' << "Round " << '\"' << ROUND[cur_round] << '\"' << "]" << endl;
	fout << '[' << "Honba " << '\"' << honba << '\"' << "]" << endl;
	fout << '[' << "Kyotaku " << '\"' << kyotaku << '\"' << "]" << endl;
	fout << '[' << "East " << '\"' << name[(0 + oya) % player_num] << '\"' << "]" << endl;
	fout << '[' << "South " << '\"' << name[(1 + oya) % player_num] << '\"' << "]" << endl;
	fout << '[' << "West " << '\"' << name[(2 + oya) % player_num] << '\"' << "]" << endl;
	if (player_num == 4)
	{
		fout << '[' << "North " << '\"' << name[(3 + oya) % player_num] << '\"' << "]" << endl;
	}
	fout << "[Style \"Horizontal\"]" << endl;
}

void round_init(void)
{
	int temp_int = 0, j = 0;
	plycnt = 0;
	prev_tile = -1;
	temp_att = cur_element->Attribute("seed");
	oya = cur_element->IntAttribute("oya");
	temp_int = 0; j = 0;
	while (temp_att[j] != ',')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	cur_round = temp_int;
	temp_int = 0;
	j++;

	while (temp_att[j] != ',')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	honba = temp_int;
	temp_int = 0;
	j++;

	while (temp_att[j] != ',')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	kyotaku = temp_int;
	temp_int = 0;
	j++;
	print_tags();

	while (temp_att[j] != ',')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	fout << '{' << "dice:" << temp_int + 1 << flush;
	temp_int = 0;
	j++;

	while (temp_att[j] != ',')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}

	fout << ',' << temp_int + 1 << '}' << endl;
	temp_int = 0;
	j++;

	while (temp_att[j] != '\0')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	dora = temp_int;
	temp_int = 0;
	j++;

}

void start_point(void)
{
	int temp_int = 0, j = 0;
	bool minus = false;

	temp_att = cur_element->Attribute("ten");
	for (size_t i = 0; i < player_num; i++)
	{
		temp_int = 0;
		minus = false;
		while (temp_att[j] != ',' && temp_att[j] != '\0')
		{
			if (temp_att[j] == '-')
			{
				minus = true;
				j++;
				continue;
			}
			(temp_int *= 10) += ((minus) ? -(temp_att[j] - '0') : (temp_att[j] - '0'));
			j++;
		}
		score[i] = temp_int;
		j++;
	}
	fout << "$Start " << score[(0 + oya) % player_num] << ' '
		<< score[(1 + oya) % player_num] << ' '
		<< score[(2 + oya) % player_num] << ' ' << flush;
	if (player_num == 4)
	{
		fout << score[(3 + oya) % player_num] << ' ' << flush;
	}
	fout << endl;
	fout << "doraind=" << ((dora % 4 || ((type >> 1) % 2)) ? TILE[dora >> 2] : TILE0[dora >> 2]) << endl;
}

int cmp(const void* lhs, const void* rhs)
{
	return (*(int*)lhs) - (*(int*)rhs);
}

void hand(void)
{
	fout << "0. " << flush;
	int temp_int = 0, k = 0;
	const char* ele_str[4] = { "hai0","hai1","hai2","hai3" };
	for (size_t i = 0; i < player_num; i++)
	{
		temp_att = cur_element->Attribute(ele_str[i]);
		temp_int = 0;
		k = 0;
		for (size_t j = 0; j < 13; j++)
		{
			temp_int = 0;
			while (temp_att[k] != ',' && temp_att[k] != '\0')
			{
				(temp_int *= 10) += (temp_att[k] - '0');
				k++;
			}
			hands[i][j] = temp_int;
			k++;
		}
	}
	for (size_t i = 0; i < player_num; i++)
	{
		qsort(hands[(i + oya) % player_num], 13, sizeof(int), cmp);
		for (size_t j = 0; j < 13; j++)
		{
			fout << ((hands[(i + oya) % player_num][j] % 4 || ((type >> 1) % 2)) ? TILE[hands[(i + oya) % player_num][j] >> 2] : TILE0[hands[(i + oya) % player_num][j] >> 2]) << flush;
		}
		fout << ' ' << flush;
	}
	fout << endl;
}

void pick_discard(void)
{
	const char* name = cur_element->Name();
	int tile = atoi(name + 1);
	switch (name[0])
	{
	case 'T': case'U': case 'V': case 'W':
		if (!(plycnt % player_num))
		{
			if ((plycnt >> 2) && !((plycnt >> 2) % 6))
			{
				fout << endl;
			}
			fout << (plycnt >> 2) + 1 << ". " << flush;
		}
		prev_tile = tile;
		fout << ((tile % 4 || ((type >> 1) % 2)) ? TILE[tile >> 2] : TILE0[tile >> 2]) << flush;
		break;
	case 'D': case'E': case 'F': case 'G':
		if (prev_tile == tile)
		{
			fout << 'x' << flush;
		}
		else
		{
			fout << ((tile % 4 || ((type >> 1) % 2)) ? TILE[tile >> 2] : TILE0[tile >> 2]) << flush;
		}
		prev_tile = tile;
		plycnt++;
		fout << ' ' << flush;

		if (new_dora)
		{
			fout << "doraind=" << ((dora % 4 || ((type >> 1) % 2)) ? TILE[dora >> 2] : TILE0[dora >> 2]) << ' ' << flush;
			new_dora = false;
		}
		break;
	}
}

void call(void)
{
	int m = cur_element->IntAttribute("m");
	if ((m & 3))
	{
		for (size_t i = 0; i < player_num - (m & 3) - 1; i++)
		{
			if ((plycnt % player_num) == 0)
			{
				if ((plycnt >> 2) && !((plycnt >> 2) % 6))
				{
					fout << endl;
				}
				fout << (plycnt >> 2) + 1 << ". " << flush;
			}
			fout << "* " << flush;
			plycnt++;
		}
		if ((plycnt % player_num) == 0)
		{
			if ((plycnt >> 2) && !((plycnt >> 2) % 6))
			{
				fout << endl;
			}
			fout << (plycnt >> 2) + 1 << ". " << flush;
		}
	}

	if (m & 0x0004)
	{
		fout << 'C' << '(' << flush;
		for (size_t i = 0; i < 3; i++)
		{
			if (i == (m >> 10) % 3)
			{
				continue;
			}
			if (m & ((3 << 3) << (i << 1)) || ((type >> 1) % 2))
			{
				fout << TILE[CHII[(m >> 10) / 3] + i] << flush;
			}
			else
			{
				fout << TILE0[CHII[(m >> 10) / 3] + i] << flush;
			}
		}
		fout << ')' << flush;
	}
	else if (m & 0x0008)
	{
		fout << 'P' << flush;
		if (!((type >> 1) % 2) && !(m & (3 << 5)) && strcmp(TILE[(m >> 9) / 3], TILE0[(m >> 9) / 3]))
		{
			if (!(m >> 9) % 3)
			{
				fout << '(' << TILE[(m >> 9) / 3] << TILE[(m >> 9) / 3] << ')' << flush;
			}
			else
			{
				fout << '(' << TILE0[(m >> 9) / 3] << TILE[(m >> 9) / 3] << ')' << flush;
			}
		}
	}
	else if (m & 0x0010)
	{
		fout << "K " << flush;
		plycnt++;
		for (size_t i = 0; i < player_num - 1; i++)
		{
			if ((plycnt % player_num) == 0)
			{
				if ((plycnt >> 2) && !((plycnt >> 2) % 6))
				{
					fout << endl;
				}
				fout << (plycnt >> 2) + 1 << ". " << flush;
			}
			fout << "* " << flush;
			plycnt++;
		}
	}
	else if (m & 0x0020)
	{
		fout << 'Z' << flush;
		plycnt++;
		for (size_t i = 0; i < player_num - 1; i++)
		{
			if ((plycnt % player_num) == 0)
			{
				if ((plycnt >> 2) && !((plycnt >> 2) % 6))
				{
					fout << endl;
				}
				fout << (plycnt >> 2) + 1 << ". " << flush;
			}
			fout << "* " << flush;
			plycnt++;
		}
	}
	else
	{
		fout << "K " << flush;
		plycnt++;
		for (size_t i = 0; i < player_num - 1; i++)
		{
			if ((plycnt % player_num) == 0)
			{
				if ((plycnt >> 2) && !((plycnt >> 2) % 6))
				{
					fout << endl;
				}
				fout << (plycnt >> 2) + 1 << ". " << flush;
			}
			fout << "* " << flush;
			plycnt++;
		}
	}
}

void	 ryuukyoku(void)
{
	const char* ele_str[4] = { "hai0","hai1","hai2","hai3" };
	temp_att = cur_element->Attribute("type");
	fout << "$Ryuukyoku " << flush;
	if (temp_att)
	{
		if (!strcmp(temp_att, "yao9"))
		{
			fout << u8"\"九種九牌\"" << endl;
		}
		else if (!strcmp(temp_att, "reach4"))
		{
			fout << u8"\"四家立直\"" << endl;
		}
		else if (!strcmp(temp_att, "ron3"))
		{
			fout << u8"\"三家和了\"" << endl;
		}
		else if (!strcmp(temp_att, "kan4"))
		{
			fout << u8"\"四槓散了\"" << endl;
		}
		else if (!strcmp(temp_att, "kaze4"))
		{
			fout << u8"\"四風連打\"" << endl;
		}
		else if (!strcmp(temp_att, "nm"))
		{
			fout << u8"\"流し満貫\"" << endl;
		}
	}
	else
	{
		fout << u8"\"流局\"" << endl;
		fout << '{' << "Tenpai: " << flush;
		for (size_t i = 0; i < player_num; i++)
		{
			if (cur_element->Attribute(ele_str[(i + oya) % 4]))
			{
				fout << SEAT[i] << ' ' << flush;
			}
		}
		fout << '}' << endl;
	}
}

void point(void)
{
	int temp_int = 0, j = 0;
	bool minus = false;
	temp_att = cur_element->Attribute("sc");
	for (size_t i = 0; i < player_num; i++)
	{
		temp_int = 0;
		minus = false;
		while (temp_att[j] != ',' && temp_att[j] != '\0')
		{
			if (temp_att[j] == '-')
			{
				minus = true;
				j++;
				continue;
			}
			(temp_int *= 10) += ((minus) ? -(temp_att[j] - '0') : (temp_att[j] - '0'));
			j++;
		}
		score[i] = temp_int;
		j++;
		temp_int = 0;
		minus = false;
		while (temp_att[j] != ',' && temp_att[j] != '\0')
		{
			if (temp_att[j] == '-')
			{
				minus = true;
				j++;
				continue;
			}
			(temp_int *= 10) += ((minus) ? -(temp_att[j] - '0') : (temp_att[j] - '0'));
			j++;
		}
		score[i] += temp_int;
		j++;
	}
	fout << "$End " << score[(0 + oya) % player_num] << ' '
		<< score[(1 + oya) % player_num] << ' '
		<< score[(2 + oya) % player_num] << ' ' << flush;
	if (player_num)
	{
		fout << score[(3 + oya) % player_num] << ' ' << flush;
	}
	fout << endl << endl;
}

void agari(void)
{
	int who = cur_element->IntAttribute("who"), from = cur_element->IntAttribute("fromWho");
	int pt = 0, rank = 0, yaku = 0, yakuman_num = 0;
	int temp_int = 0, j = 0;
	if (who == from)
	{
		fout << "$Tsumo " << flush;
	}
	else
	{
		fout << "$Ron " << flush;
	}
	fout << SEAT[(who + player_num - oya) % player_num] << ' ' << flush;
	temp_att = cur_element->Attribute("ten");
	temp_int = 0;
	j = 0;
	while (temp_att[j] != ',' && temp_att[j] != '\0')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	j++;
	fout << "fu=" << temp_int << ' ' << flush;
	temp_int = 0;
	while (temp_att[j] != ',' && temp_att[j] != '\0')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	j++;
	pt = temp_int / 100;
	temp_int = 0;
	while (temp_att[j] != ',' && temp_att[j] != '\0')
	{
		(temp_int *= 10) += (temp_att[j] - '0');
		j++;
	}
	j++;
	rank = temp_int;
	if (temp_att = cur_element->Attribute("doraHaiUra"))
	{
		j = 0;
		fout << "uraind=" << flush;
		while (true)
		{
			temp_int = 0;

			while (temp_att[j] != ',' && temp_att[j] != '\0')
			{
				(temp_int *= 10) += (temp_att[j] - '0');
				j++;
			}
			fout << ((temp_int % 4 || ((temp_int >> 1) % 2)) ? TILE[temp_int >> 2] : TILE0[temp_int >> 2]) << flush;
			if (temp_att[j] == '\0')
			{
				break;
			}
			j++;
		}
		fout << ' ' << flush;
	}
	if (temp_att = cur_element->Attribute("yakuman"))
	{
		temp_int = 0;
		j = 0;
		while (true)
		{
			temp_int = 0;
			while (temp_att[j] != ',' && temp_att[j] != '\0')
			{
				(temp_int *= 10) += (temp_att[j] - '0');
				j++;
			}
			fout << '\"' << YAKU[temp_int] << '\"' << ",Y " << flush;
			yakuman_num++;
			if (temp_att[j] == '\0')
			{
				break;
			}
			j++;
		}
	}
	else
	{
		temp_att = cur_element->Attribute("yaku");
		temp_int = 0;
		j = 0;
		while (true)
		{
			temp_int = 0;
			while (temp_att[j] != ',' && temp_att[j] != '\0')
			{
				(temp_int *= 10) += (temp_att[j] - '0');
				j++;
			}
			yaku = temp_int;
			j++;
			temp_int = 0;
			while (temp_att[j] != ',' && temp_att[j] != '\0')
			{
				(temp_int *= 10) += (temp_att[j] - '0');
				j++;
			}
			if (temp_int)
			{
				fout << '\"' << YAKU[yaku] << '\"' << ',' << temp_int << ' ' << flush;
			}
			if (temp_att[j] == '\0')
			{
				break;
			}
			j++;
		}
	}

	if (who == from)
	{
		if (who != oya)
		{
			fout << "pt=" << (pt - pt / 2) / 2 << ',' << pt / 2 << flush;
		}
		else
		{
			fout << "pt=" << pt / 3 << flush;
		}
	}
	else
	{
		fout << "pt=" << pt << flush;
	}
	if (rank)
	{
		if (rank == 5 && yakuman_num)
		{
			fout << ',' << '\"' << RANK_Y[yakuman_num] << '\"' << flush;
		}
		else
		{
			fout << ',' << '\"' << RANK[rank] << '\"' << flush;
		}
	}
	fout << endl;

}

void finish(void)
{
	int j = 0;
	if (temp_att = cur_element->Attribute("owari"))
	{
		fout << '[' << "Finish " << '\"' << flush;
		for (size_t i = 0; i < player_num; i++)
		{
			while (temp_att[j] != ',' && temp_att[j] != '\0')
			{
				j++;
			}
			j++;
			while (temp_att[j] != ',' && temp_att[j] != '\0')
			{
				fout.put(temp_att[j]);
				j++;
			}
			if (i != player_num - 1)
			{
				fout.put(',');
			}
			j++;
		}
		fout << '\"' << ']' << flush;
		fout << endl;
	}
}

int main(int argc, char* argv[])
{
	const char* ele_name;
	fout.open(argv[2], fstream::out | fstream::trunc);
	doc.LoadFile(argv[1]);
	if (!fout.is_open() || doc.ErrorID())
	{
		return 1;
	}
	game_info();
	fout << "{" << "ver" << ":" << ver->FloatValue() << "}" << endl;
	fout << "{" << "seed" << ":" << seed << "}" << endl;
	fout << "{" << "ref" << ":" << shuffle_ref << "}" << endl;

	cur_element = game = mjlog->FirstChildElement("TAIKYOKU");
	while (cur_element = cur_element->NextSiblingElement())
	{
		ele_name = cur_element->Name();
		if (!strcmp(ele_name, "INIT"))
		{
			round_init();
			start_point();
			hand();
		}
		else if (!strcmp(ele_name, "N"))
		{
			call();
		}
		else if (!strcmp(ele_name, "REACH"))
		{
			if (cur_element->IntAttribute("step") == 1)
			{
				fout << '+' << flush;
			}
		}
		else if (!strcmp(ele_name, "DORA"))
		{
			dora = cur_element->IntAttribute("hai");
			new_dora = true;
		}
		else if (!strcmp(ele_name, "AGARI"))
		{
			fout << '#' << endl;
			agari();
			point();
			finish();
		}
		else if (!strcmp(ele_name, "RYUUKYOKU"))
		{
			fout << endl;
			ryuukyoku();
			point();
			finish();
		}
		else
		{
			pick_discard();
		}
	}
	fout.close();
	return 0;
}