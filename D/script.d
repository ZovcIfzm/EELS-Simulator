import std.stdio, std.file, std.conv, std.datetime, std.math, std.parallelism, arsd.dom, arsd.script, arsd.jsvar, main;
class Script{
	Document file;
	PhaseSpace masterSpace;
	this(string fn){
		this.file = new Document(readText(fn));
	}
	void run(){
		foreach(Element t; this.file.getElementsByTagName("space")){
			this.masterSpace = new PhaseSpace(to!double(t.getAttribute("hWidth")), to!double(t.getAttribute("hHeight")), 
				to!double(t.getAttribute("VzIntDist")), to!double(t.getAttribute("zIntDist")), 
				to!double(t.getAttribute("chirp")), to!double(t.getAttribute("b")), to!double(t.getAttribute("intensity")), to!double(t.getAttribute("intensityRatio")),
				to!double(t.getAttribute("hDepth")), to!double(t.getAttribute("hDepthVelocity")), to!double(t.getAttribute("VxIntDist")), to!double(t.getAttribute("xIntDist")),
				to!double(t.getAttribute("chirpT")), to!double(t.getAttribute("bT")), to!double(t.getAttribute("vZC")), to!double(t.getAttribute("zC")), to!double(t.getAttribute("xC")));
				this.byNode(t.childNodes()[0], this.masterSpace);
		}
	}
	PhaseSpace byNode(Element statement, PhaseSpace space){
		switch (statement.tagName) {
			case "model":
				space.modelPhaseSpace(to!double(statement.getAttribute("accuracy"))); 
				break;
			case "freeexpansion":
				space.freeExpansion(to!double(statement.getAttribute("dist"))*1000000/164.35);//FIX issues with scope of creating a predefined constant 
				break;
			case "rflens":
				space.RFLens(to!double(statement.getAttribute("lenscoefficient"))*sqrt(to!double(statement.getAttribute("power"))));
				break;
			case "maglens":
				space.magLens(to!double(statement.getAttribute("lenscoefficient"))/1000000000000*pow(to!double(statement.getAttribute("power")),2));
				break;
			case "print":
				space.printPhaseSpace();
				break;
			case "specmod":
				space.specModeling();
				break;
			case "distance":
				space.freeExpansion(to!double(statement.getAttribute("dist"))*1000000/164.35);//such as double convToTime = sqrt(2*totalEnergy/(9.11*1E-31*electronAmount));
				break;
			case "analyzer":
				space.spectroscopyFunction();
				break;
			case "time":
				StopWatch sw;
				sw.start();
				this.byNode(statement.childNodes()[0], space);
				sw.stop();
				writeln("Took ",sw.peek().to!("msecs", real)(), "ms to run");
				break;
			case "split":
				auto spaces = space.split(to!long(statement.getAttribute("portions")));
				splitNumber = (to!long(statement.getAttribute("portions")));
				for (int i = 0; i < spaces.length; i++){
					spaces[i] = this.byNode(statement.childNodes()[0], spaces[i]);
				}
				//foreach ( s; spaces) s = this.byNode(statement.childNodes()[0], s);
				space = new PhaseSpace(spaces);
				break;
			case "shatter":
				auto spaces2 = space.shatter();
				if(statement.childNodes.length > 1){
					foreach (ref h; spaces2) {
						this.byNode(statement.childNodes()[0], h);
					}
				}
				space = new PhaseSpace(spaces2);
				break;
			case "pixelsum":
				space.pixelSum();
				break;
			case "recombinesumming":
				space.recombinationSumming();
				break;
			case "recombinemodeling":
				recombinationModeling();
				break;
			default: break;
		}
		if(statement.nextSibling)
			this.byNode(statement.nextSibling, space);
		return space;
	}
}
