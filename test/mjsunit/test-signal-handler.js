node.mixin(require("common.js"));

var first = 0,
    second = 0;

process.addListener('SIGUSR1', function () {
  first += 1;
});

process.addListener('SIGUSR1', function () {
  second += 1;
});

node.kill(process.pid, node.SIGUSR1);

process.addListener("exit", function () {
  assertEquals(1, first);
  assertEquals(1, second);
});
