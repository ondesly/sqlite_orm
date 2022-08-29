package com.ondesly.sqliteormtest;

import static org.junit.Assert.assertEquals;

import android.content.Context;
import android.content.res.AssetManager;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Test;
import org.junit.runner.RunWith;

@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {

    static {
        System.loadLibrary("sqliteormtest");
    }

    @Test
    public void read() {
        Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();

        AssetManager assetManager = context.getAssets();
        String result = read(assetManager);

        assertEquals("test_text", result);
    }

    public native String read(AssetManager assetManager);

}