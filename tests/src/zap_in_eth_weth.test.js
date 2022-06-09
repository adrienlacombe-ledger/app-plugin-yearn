import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu } from './test.fixture';

const BASE_SCREENS_S = (1 + 1 + 1 + 2 + 1) //YEARN + AMOUNT + VAULT + GAS_FEES + APPROVE
const BASE_SCREENS_X = (1 + 1 + 1 + 1 + 1) //YEARN + AMOUNT + VAULT + GAS_FEES + APPROVE

test('[Nano S] Zap In Eth', zemu("nanos", async (sim, eth) => {
  // Original TX: https://etherscan.io/tx/0x3fad0cdc4da1301e7e1db7b71b9419aa5e38516f75b621940dc3ce8f6496d683
  eth.signTransaction(
    "44'/60'/0'/0/0",
    "02f9035a01028473a20d008504aa71c43183077b3c948e52522e6a77578904ddd7f528a22521dc4154f5881bc16d674ec80000b902e4e69663f100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001bc16d674ec80000000000000000000000000000dcd90c7f6324cfa40d7169ef80b12031770b43250000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000018993e4a549fe3d700000000000000000000000006325440d014e39736583c165c2963ba99faf14e0000000000000000000000005ce9b49b7a1be9f2c3dc2b2a5bacea56fa21fbee0000000000000000000000000000000000000000000000000000000000000160000000000000000000000000feb4acf3df3cdea7399794d0869ef76a6efaff520000000000000000000000003ce37278de6388532c3949ce4e886f365b14fb56000000000000000000000000000000000000000000000000000000000000014464c98c6c00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000dc24316b9ae028f1497c275eb9192a3ea0f670220000000000000000000000000000000000000000000000001bc16d674ec800000000000000000000000000000000000000000000000000001a90d9e1dfe026320000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c001a04f291f7a2b4d3ab2cc48229a81d5e483a855f763ed2db6b09b88686246fdbb97a046ea417d2ce8d07310737845c45a68570649c8d565593527d1bf8b20c82df8aa"
  );

  await waitForAppScreen(sim);
  await sim.navigateAndCompareSnapshots('.', 'nanos_zapin_eth_weth', [BASE_SCREENS_S, 0]);
}));



test('[Nano X] Zap In Eth', zemu("nanox", async (sim, eth) => {
  // Original TX: https://etherscan.io/tx/0x3fad0cdc4da1301e7e1db7b71b9419aa5e38516f75b621940dc3ce8f6496d683
  eth.signTransaction(
    "44'/60'/0'/0/0",
    "02f9035a01028473a20d008504aa71c43183077b3c948e52522e6a77578904ddd7f528a22521dc4154f5881bc16d674ec80000b902e4e69663f100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001bc16d674ec80000000000000000000000000000dcd90c7f6324cfa40d7169ef80b12031770b43250000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000018993e4a549fe3d700000000000000000000000006325440d014e39736583c165c2963ba99faf14e0000000000000000000000005ce9b49b7a1be9f2c3dc2b2a5bacea56fa21fbee0000000000000000000000000000000000000000000000000000000000000160000000000000000000000000feb4acf3df3cdea7399794d0869ef76a6efaff520000000000000000000000003ce37278de6388532c3949ce4e886f365b14fb56000000000000000000000000000000000000000000000000000000000000014464c98c6c00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000dc24316b9ae028f1497c275eb9192a3ea0f670220000000000000000000000000000000000000000000000001bc16d674ec800000000000000000000000000000000000000000000000000001a90d9e1dfe026320000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c001a04f291f7a2b4d3ab2cc48229a81d5e483a855f763ed2db6b09b88686246fdbb97a046ea417d2ce8d07310737845c45a68570649c8d565593527d1bf8b20c82df8aa"
  );

  await waitForAppScreen(sim);
  await sim.navigateAndCompareSnapshots('.', 'nanox_zapin_eth_weth', [BASE_SCREENS_X, 0]);
}));


test('[Nano SP] Zap In Eth', zemu("nanosp", async (sim, eth) => {
  // Original TX: https://etherscan.io/tx/0x3fad0cdc4da1301e7e1db7b71b9419aa5e38516f75b621940dc3ce8f6496d683
  eth.signTransaction(
    "44'/60'/0'/0/0",
    "02f9035a01028473a20d008504aa71c43183077b3c948e52522e6a77578904ddd7f528a22521dc4154f5881bc16d674ec80000b902e4e69663f100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001bc16d674ec80000000000000000000000000000dcd90c7f6324cfa40d7169ef80b12031770b43250000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000018993e4a549fe3d700000000000000000000000006325440d014e39736583c165c2963ba99faf14e0000000000000000000000005ce9b49b7a1be9f2c3dc2b2a5bacea56fa21fbee0000000000000000000000000000000000000000000000000000000000000160000000000000000000000000feb4acf3df3cdea7399794d0869ef76a6efaff520000000000000000000000003ce37278de6388532c3949ce4e886f365b14fb56000000000000000000000000000000000000000000000000000000000000014464c98c6c00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000dc24316b9ae028f1497c275eb9192a3ea0f670220000000000000000000000000000000000000000000000001bc16d674ec800000000000000000000000000000000000000000000000000001a90d9e1dfe026320000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000c001a04f291f7a2b4d3ab2cc48229a81d5e483a855f763ed2db6b09b88686246fdbb97a046ea417d2ce8d07310737845c45a68570649c8d565593527d1bf8b20c82df8aa"
  );

  await waitForAppScreen(sim);
  await sim.navigateAndCompareSnapshots('.', 'nanox_zapin_eth_weth', [BASE_SCREENS_X, 0]);
}));
