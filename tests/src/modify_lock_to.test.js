import "core-js/stable";
import "regenerator-runtime/runtime";
import { waitForAppScreen, zemu, genericTx } from './test.fixture';
import { ethers } from "ethers";

const contractAddr = "0x90c1f9220d90d3966fbee24045edd73e1d588ad5";
const AMOUNT_TO_DEPOSIT = '345123456789352738273'; // 345.123456789352738273
const UNLOCK_TIME = '1727248841'; // September 25, 2024 7:20:41
const recipient = "0xB8c93dF4E1e6b1097889554D9294Dfb42814063a"; //Never use this address for anything other than tests - compromised
const BASE_SCREENS_S = (1 + 2 + 1 + 1 + 1 ) //YEARN + AMOUNT + UNLOCK_TIME + RECIPIENT + GAS_FEES + APPROVE
const BASE_SCREENS_X = (1 + 1 + 1 + 1 + 1 ) //YEARN + AMOUNT + UNLOCK_TIME + RECIPIENT + GAS_FEES + APPROVE

// Nanos S test
test('[Nano S] Modify Lock with Recipient', zemu("nanos", async (sim, eth) => {
  const contract = new ethers.Contract(contractAddr, ['function modify_lock(uint256, uint256, address)']);
  const {data} = await contract.populateTransaction.modify_lock(AMOUNT_TO_DEPOSIT, UNLOCK_TIME, recipient);
  let unsignedTx = genericTx;
  unsignedTx.to = contractAddr;
  unsignedTx.data = data;

  const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);
  const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

  await waitForAppScreen(sim);
  await sim.navigateAndCompareSnapshots('.', 'nanos_modify_lock_to', [BASE_SCREENS_S + 3, 0]);
  await tx;
}));

test('[Nano X] Modify Lock with Recipient', zemu("nanox", async (sim, eth) => {
  const contract = new ethers.Contract(contractAddr, ['function modify_lock(uint256, uint256, address)']);
  const {data} = await contract.populateTransaction.modify_lock(AMOUNT_TO_DEPOSIT, UNLOCK_TIME, recipient);

  let unsignedTx = genericTx;
  unsignedTx.to = contractAddr;
  unsignedTx.data = data;

  const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);
  const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

  await waitForAppScreen(sim);
  await sim.navigateAndCompareSnapshots('.', 'nanox_modify_lock_to', [BASE_SCREENS_X + 1, 0]);
  await tx;
}));

test('[Nano SP] Modify Lock with Recipient', zemu("nanosp", async (sim, eth) => {
  const contract = new ethers.Contract(contractAddr, ['function modify_lock(uint256, uint256, address)']);
  const {data} = await contract.populateTransaction.modify_lock(AMOUNT_TO_DEPOSIT, UNLOCK_TIME, recipient);

  let unsignedTx = genericTx;
  unsignedTx.to = contractAddr;
  unsignedTx.data = data;

  const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);
  const tx = eth.signTransaction("44'/60'/0'/0", serializedTx);

  await waitForAppScreen(sim);
  await sim.navigateAndCompareSnapshots('.', 'nanosp_modify_lock_to', [BASE_SCREENS_X + 1, 0]);
  await tx;
}));
