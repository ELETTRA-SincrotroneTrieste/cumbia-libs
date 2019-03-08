function (a, b) {
    var c = [];
    for(var i = 0; i < a.length && i < b.length; i++) {
        c.push(a[i] + b[i]);
    }
    return c;
}

