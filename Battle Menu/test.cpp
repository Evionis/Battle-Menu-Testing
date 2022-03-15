#include <math.h>
#include <string.h>
#include <cstdlib>
#include "pch.h"

using namespace std;

struct effect
{
	char effectType = 'z'; //a = armor b = bonus damage m = damage multiplier f = freeze s = spectral v = vulnerable
	double strength = -1;
};

struct attack
{
	int baseDmg;
	char dmgType;
	effect effect;
};

struct skill
{
	string name = "";
	effect effect;
};

struct weapon
{
	string name;
	char dmgType;
	int dmgBonus;
	effect effect;
};

struct entity
{
	string name = "";
	int hp = -1, att = -1, def = -1;
	double sRes = -1, cRes = -1, pRes = -1; //slashing resistance %, crushing resistance %, piercing resistance %
	weapon weapon;
	skill skills[3];
	int skillCount = 0;
	effect effect;
};

char checkResult(entity player, entity enemy);
void dealDamage(entity& target, attack attack);
void doBattle(entity& player, entity& enemy);
attack generateAttack(entity attacker);
entity generateEnemy(int difficulty);
weapon generateWeapon();
char takeTurn(entity& player, entity& enemy);
void useSkill(entity& target, entity attacker, int skillNum);

TEST(TestDealDamage, TestRawDamage) {
	entity target{ "", 20, 0, 0, 0, 0, 0, { "Sword", 's', 1 } };
	attack attack{ 11, 's' };
	dealDamage(target, attack);
	ASSERT_EQ(target.hp, 9);
}

TEST(TestDealDamage, TestModifiedDamage) {
	entity target{ "", 30, 0, 1, 0, 0, 0.2, { "Sword", 's', 1 } };
	attack attack{ 10, 'p' };
	dealDamage(target, attack);
	ASSERT_EQ(target.hp, 23);
}

TEST(TestCheckResult, TestWin) {
	entity player{ "", 30, 3, 1, 0.1, 0.2, 0.1, { "Sword", 's', 1 } };
	entity enemy{ "", -2, 2, 2, 0, 0.3, 0, { "Sword", 's', 1 } };
	char result = checkResult(player, enemy);
	ASSERT_TRUE(result == 'w');
}

TEST(TestCheckResult, TestLoss) {
	entity player{ "", -1, 3, 1, 0.1, 0.2, 0.1, { "Sword", 's', 1 } };
	entity enemy{ "", 7, 2, 2, 0, 0.3, 0, { "Sword", 's', 1 } };
	char result = checkResult(player, enemy);
	ASSERT_TRUE(result == 'l');
}

TEST(TestCheckResult, TestTie) {
	entity player{ "", -2, 3, 1, 0.1, 0.2, 0.1, { "Sword", 's', 1 } };
	entity enemy{ "", -3, 2, 2, 0, 0.3, 0, { "Sword", 's', 1 } };
	char result = checkResult(player, enemy);
	ASSERT_TRUE(result == 't');
}

TEST(TestCheckResult, TestContinue) {
	entity player{ "", 15, 3, 1, 0.1, 0.2, 0.1, { "Sword", 's', 1 } };
	entity enemy{ "", 12, 2, 2, 0, 0.3, 0, { "Sword", 's', 1 } };
	char result = checkResult(player, enemy);
	ASSERT_TRUE(result == 'c');
}

TEST(TestGenerateAttack, Test1) {
	entity player{ "", 15, 3, 1, 0.1, 0.2, 0.1, { "Sword", 's', 1 } };
	attack expected = { 4, 's' };
	attack test = generateAttack(player);
	EXPECT_EQ(expected.baseDmg, test.baseDmg);
	EXPECT_EQ(expected.dmgType, test.dmgType);
}

TEST(TestGenerateWeapon, Test1) {
	weapon test = generateWeapon();
	EXPECT_NE(test.name, "") << "Weapon name not generated";
	EXPECT_NE(test.dmgType, 'u') << "Weapon damage type not generated";
	EXPECT_NE(test.dmgBonus, -1) << "Weapon bonus damage not generated";
}

TEST(TestGenerateEnemy, Test1) {
	entity test = generateEnemy(1);

	EXPECT_NE(test.hp, -1) << "Entity hp not generated";
	EXPECT_NE(test.att, -1) << "Entity att not generated";
	EXPECT_NE(test.def, -1) << "Entity def not generated";
	EXPECT_NE(test.sRes, -1) << "Entity sRes not generated";
	EXPECT_NE(test.cRes, -1) << "Entity cRes not generated";
	EXPECT_NE(test.pRes, -1) << "Entity pRes not generated";

	EXPECT_NE(test.weapon.name, "") << "Weapon name not generated";
	EXPECT_NE(test.weapon.dmgType, 'u') << "Weapon damage type not generated";
	EXPECT_NE(test.weapon.dmgBonus, -1) << "Weapon bonus damage not generated";
}

TEST(TestTakeTurn, Attack) {
	entity player{ "Glorious Hero", 50, 4, 1, 0.1, 0.2, 0.1, { "Holy Sword", 's', 3 } };
	entity playerCopy = player;
	entity test = generateEnemy(2);
	entity testCopy = test;
	takeTurn(player, test);
	EXPECT_LT(test.hp, testCopy.hp);
	EXPECT_LT(player.hp, playerCopy.hp);
}

TEST(TestTakeTurn, Skill) {
	entity player{ "Glorious Hero", 50, 4, 1, 0.1, 0.2, 0.1, { "Holy Sword", 's', 3 } };
	player.skills[0] = { "Power Slash", {'m', 1.5} };
	player.skillCount = 1;
	entity playerCopy = player;
	entity test = generateEnemy(2);
	entity testCopy = test;
	takeTurn(player, test);
	EXPECT_LT(test.hp, testCopy.hp);
	EXPECT_LT(player.hp, playerCopy.hp);
	EXPECT_GT(test.skillCount, 0);
	EXPECT_NE(test.skills[0].name, "");
}

TEST(TestTakeTurn, SkillFrozen) {
	entity player{ "Glorious Hero", 50, 4, 1, 0.1, 0.2, 0.1, { "Holy Sword", 's', 3 } };
	player.skills[0] = { "Frozen Slash", {'f', 1} };
	player.skillCount = 1;
	entity playerCopy = player;
	entity test = generateEnemy(2);
	entity testCopy = test;
	takeTurn(player, test);
	EXPECT_LT(test.hp, testCopy.hp);
	EXPECT_EQ(player.hp, playerCopy.hp);
	EXPECT_GT(test.skillCount, 0);
	EXPECT_NE(test.skills[0].name, "");
}

TEST(TestDoBattle, Test1) {
	entity player{ "Glorious Hero", 50, 4, 1, 0.1, 0.2, 0.1, { "Holy Sword", 's', 5 } };
	player.skills[0] = { "Power Slash", {'m', 1.5} };
	player.skills[1] = { "Frozen Slash", {'f', 1} };
	player.skillCount = 2;
	entity playerCopy = player;
	entity test = generateEnemy(1);
	entity testCopy = test;
	doBattle(player, test);
	EXPECT_LT(test.hp, testCopy.hp);
	EXPECT_LT(player.hp, playerCopy.hp);
	if (test.hp <= 0) {
		EXPECT_LE(test.hp, 0);
	}
	else {
		EXPECT_LE(player.hp, 0);
	}
}

TEST(TestGenerateWeapon, TestEffect) {
	weapon test = generateWeapon();
	EXPECT_NE(test.name, "") << "Weapon name not generated";
	EXPECT_NE(test.dmgType, 'u') << "Weapon damage type not generated";
	EXPECT_NE(test.dmgBonus, -1) << "Weapon bonus damage not generated";
	EXPECT_NE(test.effect.effectType, 'z') << "Weapon effect not generated";
}

TEST(TestGenerateEnemy, TestEffect) {
	entity test = generateEnemy(1);

	EXPECT_NE(test.hp, -1) << "Entity hp not generated";
	EXPECT_NE(test.att, -1) << "Entity att not generated";
	EXPECT_NE(test.def, -1) << "Entity def not generated";
	EXPECT_NE(test.sRes, -1) << "Entity sRes not generated";
	EXPECT_NE(test.cRes, -1) << "Entity cRes not generated";
	EXPECT_NE(test.pRes, -1) << "Entity pRes not generated";

	EXPECT_NE(test.weapon.name, "") << "Weapon name not generated";
	EXPECT_NE(test.weapon.dmgType, 'u') << "Weapon damage type not generated";
	EXPECT_NE(test.weapon.dmgBonus, -1) << "Weapon bonus damage not generated";

	EXPECT_NE(test.skills[0].effect.effectType, 'z') << "Entity effect not generated";
}

char checkResult(entity player, entity enemy)
{
	if (player.hp > 0 && enemy.hp <= 0)
		return 'w';
	else if (player.hp <= 0 && enemy.hp > 0)
		return 'l';
	else if (player.hp <= 0 && enemy.hp <= 0)
		return 't';
	return 'c';
}

void dealDamage(entity& target, attack attack)
{
	int finalDmg = attack.baseDmg;
	if (attack.effect.effectType == 'v') {
		target.sRes -= attack.effect.strength;
		target.cRes -= attack.effect.strength;
		target.pRes -= attack.effect.strength;
	}
	switch (attack.dmgType)
	{
		case 's':
			finalDmg = int(floor(finalDmg * (1 - target.sRes)));
			break;
		case 'c':
			finalDmg = int(floor(finalDmg * (1 - target.cRes)));
			break;
		case 'p':
			finalDmg = int(floor(finalDmg * (1 - target.pRes)));
			break;
	}
	finalDmg = finalDmg - target.def;
	if (finalDmg < 1) {
		finalDmg = 1;
	}
	if (target.skills[0].effect.effectType == 's') {
		int chance = rand() % 10;
		if (chance <= 3) {
			finalDmg = 1;
			cout << target.name << " avoids damage due to it's spectral perk." << endl;
		}
	}
	target.hp = target.hp - finalDmg;
	cout << target.name << " takes " << finalDmg << " HP of damage." << endl;
}

void doBattle(entity& player, entity& enemy)
{
	bool fighting = true;
	char result;
	while (fighting) {
		result = takeTurn(player, enemy);
		switch (result) {
			case 'w':
				cout << "The Hero triumphs over their enemy once more." << endl;
				fighting = false;
				break;
			case 'l':
				cout << "The Hero falls to the ground, dead." << endl;
				fighting = false;
				break;
			case 't':
				cout << "The Hero and their enemy lie atop one another in a pool of blood." << endl;
				fighting = false;
				break;
			case 'c':
				break;
			default:
				cout << "Something went wrong, unexpected result received from takeTurn." << endl;
				fighting = false;
				break;
		}
	}
}

attack generateAttack(entity attacker)
{
	char dmgType = attacker.weapon.dmgType;
	int baseDmg = attacker.att + attacker.weapon.dmgBonus;
	attack newAttack = { baseDmg, dmgType };
	if (attacker.skills[0].effect.effectType == 'b') {
		newAttack.baseDmg += attacker.skills[0].effect.strength;
	}
	if (attacker.weapon.effect.effectType == 'b') {
		newAttack.baseDmg += attacker.skills[0].effect.strength;
	}
	else if (attacker.weapon.effect.effectType == 'v') {
		newAttack.effect = attacker.weapon.effect;
	}
	return newAttack;
}

entity generateEnemy(int difficulty)
{
	string names[] = { "Naga", "Golem", "Lost Spirit" };
	skill skills[] = { { "Viscious", { 'b', 1 } }, { "Heavily Armored", { 'a', 1 } }, { "Spectral", { 's', 0 } } };
	int choice = rand() % 3;
	entity newEnemy;
	newEnemy.name = names[choice];
	newEnemy.hp = 20 + (rand() % 10 * difficulty);
	newEnemy.att = 1 + (rand() % 2 * difficulty);
	newEnemy.def = 0 + (rand() % 2 * difficulty);
	newEnemy.sRes = 0.0 + ((((rand() % 5) / 10.0) * difficulty / 2) - 0.2);
	newEnemy.cRes = 0.0 + ((((rand() % 5) / 10.0) * difficulty / 2) - 0.2);
	newEnemy.pRes = 0.0 + ((((rand() % 5) / 10.0) * difficulty / 2) - 0.2);
	newEnemy.weapon = generateWeapon();
	newEnemy.skills[0] = skills[choice];
	newEnemy.skillCount = 1;
	return newEnemy;
}

weapon generateWeapon()
{
	weapon slashers[] = { { "Training Sword", 's', 1 }, { "Longsword", 's', 2 } };
	weapon crushers[] = { { "Wooden Club", 'c', 1 }, { "Stone Club", 'c', 2 } };
	weapon piercers[] = { { "Wooden Spear", 'p', 1 }, { "Stone Spear", 'p', 2 } };
	effect effects[] = { { 'd', 1 }, { 'v', .1 } };
	int uniqueChance = (rand() % 100) + 1;
	int effectChance = (rand() % 100) + 1;
	int weaponType = rand() % 3;
	int weaponChoice = rand() % 2; // value should be adjusted for number of options in the lists
	int effectChoice = rand() % 2; // value should be adjusted for number of options in the lists
	if (uniqueChance <= 100) { // 15% chance of unique weapon
		string nameList[] = { " of Joy", " of Rage", " of Pain", " of Sin" };
		weapon baseWeapon;
		switch (weaponType) {
			case 0: //basic slashing weapon
				baseWeapon = slashers[weaponChoice];
				break;
			case 1: //basic crushing weapon
				baseWeapon = crushers[weaponChoice];
				break;
			case 2: //basic piercing weapon
				baseWeapon = piercers[weaponChoice];
				break;
		}
		int nameIndex = rand() % 4;
		int dmgBonus = rand() % 3 + 1; // 1-3 bonus dmg
		weapon uniqueWeapon = { baseWeapon.name += nameList[nameIndex], baseWeapon.dmgType, baseWeapon.dmgBonus + dmgBonus };
		if (effectChance <= 100) { // 30% chance of effect
			uniqueWeapon.effect = effects[effectChoice];
		}
		return uniqueWeapon;
	}
	else {
		switch (weaponType) {
			case 0: //basic slashing weapon
				return slashers[weaponChoice];
				break;
			case 1: //basic crushing weapon
				return crushers[weaponChoice];
				break;
			case 2: //basic piercing weapon
				return piercers[weaponChoice];
				break;
		}
	}
	return weapon{ "", 'u', -1 };
}

char takeTurn(entity& player, entity& enemy)
{
	cout << enemy.name << endl;
	cout << "HP: " << enemy.hp << endl;
	cout << "Atk: " << enemy.att;
	if (enemy.skills[0].effect.effectType == 'b') {
		cout << " + " << enemy.skills[0].effect.strength;
	}
	cout << endl;
	cout << "Def: " << enemy.def;
	if (enemy.skills[0].effect.effectType == 'a') {
		cout << " + " << enemy.skills[0].effect.strength;
	}
	cout << endl;
	cout << "sRes: " << enemy.sRes << endl;
	cout << "cRes: " << enemy.cRes << endl;
	cout << "pRes: " << enemy.pRes << endl;
	cout << "\n";
	cout << player.name << endl;
	cout << "HP: " << player.hp << endl;
	cout << "Atk: " << player.att << endl;
	cout << "Def: " << player.def << endl;
	cout << "sRes: " << player.sRes << endl;
	cout << "cRes: " << player.cRes << endl;
	cout << "pRes: " << player.pRes << endl;
	cout << "\n";
	cout << "(A)ttack	(S)kill" << endl;
	char input;
	cin >> input;
	switch (toupper(input)) {
		case 'A':
			dealDamage(enemy, generateAttack(player));
			break;
		case 'S':
			for (int i = 0; i < player.skillCount; i++) {
				cout << (i + 1) << ": " << player.skills[i].name << endl;
			}
			int skillInput;
			cin >> skillInput;
			useSkill(enemy, player, skillInput - 1);
			break;
		default:
			cout << "You stumble and miss your turn!" << endl;
			break;
	}
	if (enemy.effect.effectType == 'f' && enemy.effect.strength > 0) {
		enemy.effect.strength -= 1;
		cout << enemy.name << " is frozen solid!" << endl;
	}
	else {
		dealDamage(player, generateAttack(enemy));
	}
	return checkResult(player, enemy);
}

void useSkill(entity& target, entity attacker, int skillNum)
{
	attack modified;
	switch (attacker.skills[skillNum].effect.effectType) {
		case 'm':
			modified = generateAttack(attacker);
			modified.baseDmg = modified.baseDmg * attacker.skills[skillNum].effect.strength;
			cout << attacker.name << " uses " << attacker.skills[skillNum].name << "!" << endl;
			dealDamage(target, modified);
			break;
		case 'f':
			target.effect = { 'f', 1 };
			dealDamage(target, generateAttack(attacker));
			break;
	}
}

int main(int argc, char** argv)
{
	srand(time(0));
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}