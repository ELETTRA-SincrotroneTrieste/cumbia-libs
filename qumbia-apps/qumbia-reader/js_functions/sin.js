(function (_siz, min, max, period, call_cnt, last_val) {
    amplitude = Math.random() * (max - min) + min;
    angle = Math.random() * 360.0;
    angle = 2 * Math.PI * angle / 360.0;
    if(_siz === 1) {
        return amplitude * Math.sin(angle);
    }
    else {
        step = 2.0 * Math.PI / 20.0;
        var arr = []
        for(i = 0; i < _siz; i++) {
            arr.push(amplitude * Math.sin(step * i));
        }
//        console.log(arr);
        return arr;
    }
})
