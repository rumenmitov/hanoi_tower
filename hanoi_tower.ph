package .ru.dz.demo;

import .phantom.os;
import .internal.io.tty;
import .internal.long;
import .internal.double;
import .internal.float;
import .internal.string;
import .internal.wasm;


class hanoi_tower {
    void run(var console : .internal.io.tty)
    {
        var wamr : .internal.wasm;
        var args : .internal.object[];

        wamr = new .internal.wasm();
        args = new .internal.object[]();

        wamr.loadModule(getHanoiTowerBinary());
        
        args[0] = wamr.shareObject(getBgAsset());
        args[1] = wamr.shareObject(getRedBlockAsset());
        args[2] = wamr.shareObject(getGreenBlockAsset());
        args[3] = wamr.shareObject(getBlueBlockAsset());

        wamr.invokeWasm("hanoi_tower", args);
        
        console.putws("Hanoi Tower!\n");
    }

    .internal.string getHanoiTowerBinary()
    {
        return import "src/hanoi_tower/assets/hanoi_tower.wasm";
    }

    .internal.string getBgAsset()
    {
        return import "src/hanoi_tower/assets/bg.jpg" ;
    }

    .internal.string getRedBlockAsset()
    {
        return import "src/hanoi_tower/assets/red_block.jpg";
    }

    .internal.string getGreenBlockAsset()
    {
        return import "src/hanoi_tower/assets/green_block.jpg";
    }

    .internal.string getBlueBlockAsset()
    {
        return import  "src/hanoi_tower/assets/blue_block.jpg";
    }
};
