node.mixin(require("common.js"));

if (process.ARGV[2] === "-child") {
  
  var handler = new node.SignalHandler(node.SIGUSR1);
  handler.addListener("signal", function() {
    print("handled SIGUSR1");
    handler.stop();
  });
  
} else {
  
  var command = process.ARGV[0] + ' -- ' + process.ARGV[1] + ' -child';
  
  var child = node.createChildProcess(command);

  var output = "";
  
  child.addListener('output', function (chunk) {
    if (chunk) { output += chunk };
  });
  
  setTimeout(function() { child.kill(node.SIGUSR1) }, 1000);

  process.addListener("exit", function () {
    assertEquals("handled SIGUSR1", output);
  });
  
}