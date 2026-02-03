import { NitroModules } from 'react-native-nitro-modules';
import type { NitroState } from './NitroState.nitro';

const NitroStateHybridObject =
  NitroModules.createHybridObject<NitroState>('NitroState');

export function multiply(a: number, b: number): number {
  return NitroStateHybridObject.multiply(a, b);
}
