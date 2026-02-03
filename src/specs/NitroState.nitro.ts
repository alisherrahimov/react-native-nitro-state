import type { AnyMap, HybridObject } from 'react-native-nitro-modules';

/**
 * NitroState - C++ backed fine-grained state management
 *
 * Low-level interface to the C++ state engine.
 * Use the higher-level `atom()` and hooks instead of this directly.
 */
export interface NitroState
  extends HybridObject<{ ios: 'c++'; android: 'c++' }> {
  // ----- Atom Operations -----

  /**
   * Create a new atom with initial value
   */
  createAtom(key: string, initialValue: AnyMap): void;

  /**
   * Get current atom value
   */
  getAtomValue(key: string): AnyMap;

  /**
   * Set atom value
   */
  setAtomValue(key: string, value: AnyMap): void;

  /**
   * Subscribe to atom changes
   * @returns Unsubscribe function
   */
  subscribeAtom(key: string, callback: () => void): () => void;

  /**
   * Delete an atom
   */
  deleteAtom(key: string): void;

  // ----- Computed Operations -----

  /**
   * Create a computed value from dependencies
   */
  createComputed(
    key: string,
    dependencies: string[],
    compute: () => AnyMap
  ): void;

  /**
   * Get computed value
   */
  getComputedValue(key: string): AnyMap;

  /**
   * Delete a computed value
   */
  deleteComputed(key: string): void;

  // ----- Batch Operations -----

  /**
   * Start a batch operation (defers notifications)
   */
  startBatch(): void;

  /**
   * End batch and notify all pending subscribers
   */
  endBatch(): void;

  // ----- Utility -----

  /**
   * Check if an atom exists
   */
  hasAtom(key: string): boolean;

  /**
   * Get all atom keys
   */
  getAtomKeys(): string[];
}
