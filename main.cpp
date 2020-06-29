#include <dllentry.h>
#include "base/log.h"
#include "settings.h"
#include "base/playerdb.h"
#include "../BuiltinMods/Audit/include/mods/Audit.h"
#include "Block/VanillaBlocks.h"
#include "Block/VanillaBlockTypes.h"
#include "Block/BlockSource.h"
#include "Core/HashedString.h"
#include "Block/BlockLegacy.h"
#include "Block/VanillaBlockTypes.h"
#include "Block/BlockPalette.h"
#include "../Public/hook.h"

#define VA unsigned long long
DEF_LOGGER("MxOreTeleport");
DEFAULT_SETTINGS(settings);
struct NewBlockID {
  unsigned short value;
  NewBlockID(unsigned short v) : value(v) {}
};
void checkDestroy(
    Mod::PlayerEntry const &entry, ComplexInventoryTransaction const &transaction,
    Mod::CallbackToken<std::string> &token);
    // PS: Reserved for compatibility purposes
// If it is not necessary, keep both functions empty
// Initialization can be done in the PreInit function
// Incorrect use of this function may cause a crash
void dllenter() {}
void dllexit() {}
std::string getBlockName(const BlockLegacy &legacy) {
  HashedString *hs = (HashedString *) ((__int64) &legacy + 104);
  std::string str  = hs->getString();
  return str;
}
struct BPos3 {
  INT32 x;
  INT32 y;
  INT32 z;
  BlockPos *getBlockPos() { return reinterpret_cast<BlockPos*>(reinterpret_cast<VA>(this));
  }
  BPos3(int tx, int ty, int tz) : x(tx), y(ty), z(tz) {}

};

BPos3* getPosition(BlockPos* pblkpos) { return reinterpret_cast<BPos3 *>(reinterpret_cast<VA>(pblkpos)); }
bool testStone(BlockSource &bls, BlockPos blkpos) {
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock() )== "minecraft:stone")
    return true;
  else
    return false;
}
bool testOre(BlockSource& bls, BlockPos blkpos) {
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock()) == "minecraft:iron_ore")
    return true;
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock()) == "minecraft:gold_ore") return true;
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock()) == "minecraft:diamond_ore") return true;
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock()) == "minecraft:lapis_ore") return true;
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock()) == "minecraft:redstone_ore") return true;
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock()) == "minecraft:coal_ore") return true;
  if (getBlockName(bls.getBlock(blkpos).getLegacyBlock()) == "minecraft:emerald_ore") return true;
    return false;
}

void PreInit() {
  LOGV("MxOreTeleport");
  // You can use the event system to receive and process events
  // The following is an example
  Mod::AuditSystem::GetInstance().AddListener(SIG("inventory_transaction"), {Mod::RecursiveEventHandlerAdaptor(checkDestroy)});


}
void PostInit() { LOGV("post init"); }

void checkDestroy(
    Mod::PlayerEntry const &entry, ComplexInventoryTransaction const &transaction,
    Mod::CallbackToken<std::string> &token) {

    {
    auto bl      = getBlockName(*VanillaBlockTypes::mStone);
    auto &de = (VanillaBlockTypes::mStone)->getDefaultState();
    auto id = VanillaBlockTypes::mStone->BlockID;
   
    switch (transaction.type) {
    case ComplexInventoryTransaction::Type::ITEM_USE: {
      auto &data = (ItemUseInventoryTransaction const &) transaction;
      switch (data.actionType) {
      case ItemUseInventoryTransaction::Type::USE_ITEM_ON:
        
        break;
      case ItemUseInventoryTransaction::Type::USE_ITEM: break;
      case ItemUseInventoryTransaction::Type::DESTROY:
       {
         
          BlockPos pos = data.pos;
          
          LOGV("debug");
          auto &bs         = entry.player->Region;
          auto &block = entry.player->Region.getBlock(data.pos);
          auto &legacy     = block.getLegacyBlock();
          HashedString* hs = (HashedString*)((__int64)&legacy + 104);
          std::string str  = hs->getString();
          std::vector<BPos3> dir;
          dir.push_back(BPos3(pos.x, pos.y-1, pos.z));
          std::string name = getBlockName(entry.player->Region.getBlock(*dir[0].getBlockPos()).getLegacyBlock());

        //  bs.setBlock(pos, de, 3, nullptr);
          /*LOGV((char *) name.c_str());*/
          dir.push_back(BPos3(pos.x, pos.y+1, pos.z));
          dir.push_back(BPos3(pos.x+1, pos.y, pos.z));
          dir.push_back(BPos3(pos.x-1, pos.y, pos.z));
          dir.push_back(BPos3(pos.x, pos.y, pos.z+1));
          dir.push_back(BPos3(pos.x, pos.y, pos.z-1));
          for (int i = 0; i < 6; i++) {
            if (testStone(entry.player->Region, *dir[i].getBlockPos()) ||
                testOre(entry.player->Region, *dir[i].getBlockPos())) {
              auto tmp = BPos3(dir[i].x + settings.xoffset, dir[i].y + settings.yoffset, dir[i].z + settings.zoffset);
              auto tmp2 = BPos3(dir[i].x, dir[i].y -2, dir[i].z );
              if (testStone(entry.player->Region, *tmp.getBlockPos()) ||
                  testOre(entry.player->Region, *tmp.getBlockPos())) {
                bs.setBlock(*dir[i].getBlockPos(), bs.getBlock(*tmp.getBlockPos()), 3, nullptr);
                bs.setBlock(*tmp.getBlockPos(), de, 3, nullptr);
              } else {
                if (testStone(entry.player->Region, *tmp2.getBlockPos()) ||
                    testOre(entry.player->Region, *tmp2.getBlockPos())) {
                  bs.setBlock(*dir[i].getBlockPos(), bs.getBlock(*tmp2.getBlockPos()), 3, nullptr);
                  bs.setBlock(*tmp2.getBlockPos(), de, 3, nullptr);
                }
              }
            
            }
          
          }
          /*auto bname  = pblock.getLegacyBlock().getBlockID();
          std::cout << bname;*/
          
          //std::cout << str;


          data.handle(*entry.player,true);
          token("continue");


          
          break;
        }
        break;
      }
    } break;
    case ComplexInventoryTransaction::Type::ITEM_USE_ON_ACTOR: {

      
    } break;
    default: break;
    }
  }
}