package com.margelo.nitro.nitrostate
  
import com.facebook.proguard.annotations.DoNotStrip

@DoNotStrip
class NitroState : HybridNitroStateSpec() {
  override fun multiply(a: Double, b: Double): Double {
    return a * b
  }
}
