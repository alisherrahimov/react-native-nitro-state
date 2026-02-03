import { NitroModules } from 'react-native-nitro-modules';
import type { NitroState } from '../specs/NitroState.nitro';

/**
 * Singleton instance of the C++ NitroState HybridObject
 */
let _instance: NitroState | null = null;

/**
 * Get the NitroState C++ instance
 */
export function getNitroState(): NitroState {
  if (!_instance) {
    _instance = NitroModules.createHybridObject<NitroState>('NitroState');
  }
  return _instance;
}

/**
 * Reset the instance (for testing purposes)
 */
export function resetNitroState(): void {
  _instance = null;
}
