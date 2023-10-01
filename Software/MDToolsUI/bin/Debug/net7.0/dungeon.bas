90 LBYTES flp1_title,131072
95 PAUSE
100 CLEAR
110 RESTORE
120 WINDOW #2,512,256,0,0
130 INK #2,0:PAPER #2,0
140 WINDOW 448,240,32,16
150 instructIons
160 RANDOMISE
170 initialize
180 GO TO 4870
190 WINDOW #0,448,90,32,16
200 WINDOW #1,448,80,32,106
210 WINDOW #2,448,70,32,186
220 PAPER #0,1:INK #0,7
230 PAPER 0:INK 5
240 PAPER #2,2:INK #2,6
250 CLS #0:CLS:CLS #2
260 STAT
270 n1=n:s1=s:e1=e:w1=w:d1=d:u1=u:nw1=nw:sw1=sw:ne1=ne:se1=se
280 jusmov=0:tre=0:gold=0
290 n=0:s=0:u=0:d=0:w=0:e=0:sw=0:se=0:ne=0:nw=0
300 mo=0
310 SELect ON po
320 =10:PRINT #0," You are standing at the top of a    rough flight of stone steps.They     descend steeply into the dark depths of the dungeon of ORNAK!":d=1
330 =110:PRINT #0," You are at the bottom of some stone steps.It is very dark.The passage    leads north.":u=1:n=1:IF RND(3)=1THEN mo=RND(3)
340 =120:PRINT #0,ns$:n=1:s=1:IF RND(3)=1THEN mo=RND(3)
350 =130:PRINT #0,ns$:n=1:s=1:IF RND(2)=1THEN mo=RND(2)
360 =140:PRINT #0," You are standing in a long passage  which heads north and south.On eitherside of you is a door.":n=1:s=1:w=1:e=1:IF RND(2)=1 THEN mo=1
370 IF pp=1THEN PRINT #0,"The east door has been smashed!"
380 =139:PRINT #0," You are in a small square room.Thereis a table and chair at its centre.";:e=1
390 IF od=0 THEN PRINT#0,"Onthe chair is a surprised and drunken looking ORC.Under the chair is a box.":mo=1
400 IF od=1 THEN PRINT#0,"Onthe chair are the disgusting remains of an ORC.There is blood everywhere. You can see a small box.":IF RND(1 TO 3)=1THEN mo=RND(1 TO 3)
410 =141:PRINT #0," You are in a small square,empty room":w=1:n=1:IF RND(5)=1THEN mo=RND(2)
420 =150:PRINT #0,ns$:n=1:s=1:IF RND(7)=1THEN mo=RND(3)
430 =151:PRINT #0,ns$:n=1:s=1:IF RND(2)=1THEN mo=1
440 =160:PRINT #0,ns$:n=1:s=1:IF RND(2)=1THEN mo=RND(3)
450 =161:PRINT #0,ns$:n=1:s=1:IF RND(5)=1THEN mo=RND(4)
460 =170:PRINT #0," You are at a corner in the passage, which leads west and south.The air isthick with the stench of rotting     flesh.":n=1:w=1:s=1:IF RND(5)=1 THEN mo=2
470 =169:PRINT #0," You are in a wide,west-east passage.":e=1
480 IF id=0THEN PRINT#0,"Your way west is blocked by a huge,  barred,iron door."
490 IF id=1THEN PRINT#0,"You can see the bottom of the iron   door in the roof of the passage!":w=1
500 =168:PRINT #0," You are at a turn in the passage.Oneway goes north,another goes east.":e=1:n=1:IF RND(5)=1THEN mo=RND(3)
510 IF id=0THEN PRINT#0," Unfortunately you cannot go east.   Your way is blocked by an iron door.":e=0
520 =178:PRINT #0," The passage goes north and south.To your west you can just make out a    throne,positioned on the far side of an enormous room.":n=1:s=1:w=1:IF RND(3)=1THEN mo=RND(3)
530 =177:PRINT #0," You are in the middle of a large    room.To your west is a magnificent   throne.":w=1:e=1
540 IF RND(3)=1 THEN mo=RND(3)
550 IF sk=0 THEN mo=3
560 =176:PRINT #0," You are on a small platform,next to a very large throne.The only exit is east.":e=1:IF RND(3)=1 THEN mo=RND(3)
570 =187:PRINT #0," You are sitting on a large throne,  identical to the previous room,exceptthat to your north are squares.Beyondthe squares sits a large chest.The   squares are of one colour,either red,blue or green.":n=1
580 =197:PRINT #0," You have crossed the squares and arenow in a large,dull room.Sitting in  your midst is a large,iron bound     chest.There is a distinct lack of    monsters!":s=1
590 =188:PRINT #0," You are in a narrow passage which   turns south and east.":s=1:e=1:IF RND(2)=1 THEN mo=RND(2)
600 =189:PRINT #0," You are in a bendy,narrow,low       ceilinged passage.It seems to go eastand west for a long time.":e=1:w=1:IF RND(2)=1THEN mo=RND(5)
610 =190:PRINT #0," You are at a sharp turn in the      passage.It heads west and south into the gloom.":w=1:s=1:IF RND(5)=1THEN mo=RND(4)
620 =180:PRINT #0," You are in a very narrow passage.Thewalls are covered in stinking grime. Every now and then you here a        terrifying wail.The passage goes     north and south.":n=1:s=1:IF RND(5)=1THEN mo=RND(2)
630 =171:PRINT #0," The passage,which was running north,turns east.":e=1:s=1
640 =172:PRINT #0," The passage goes east and west.":w=1:e=1:mo=RND(2)
650 =173:PRINT #0," You are at a T junction.The path    from the east has joined with a large,north-south passage.":n=1:s=1:w=1:IF RND(8)=1THEN mo=RND(6)
660 =163:PRINT #0,ns$:n=1:s=1:IF RND(5)=1THEN mo=RND(5)
670 =153:PRINT #0," You are at the western most end of atruly massive room.There is a table  running down its centre.In the near  corner you can see a small skelaton  propped up by some chains.":n=1:e=1
680 IF os=0THEN mo=1
690 =154:PRINT #0," You are at the eastern end of the   room.";:w=1
700 IF oc=0THEN mo=1:PRINT
710 IF mt=0THEN PRINT #0,"There is a man tied up in the   corner.He looks hungry and depressed but he isn't frightened."
720 =183:PRINT #0,ns$:n=1:s=1:IF RND(4)=1THEN mo=RND(3)
730 =193:PRINT #0," You are at a crossroads of four     passages.":n=1:s=1:e=1:w=1:IF RND(4)=1THEN mo=RND(2)
740 =194:PRINT #0," You are in a large,low ceilinged    room.There are bones strewn all over the floor,some have been smashed,    others still have flesh in places.":w=1:IF RND(3)=1 THEN mo=RND(2)
750 IF ra=0 THEN mo=4
760 =203:PRINT #0," You are in a small,square pit about 10 feet deep.The walls are very      slippery,you can't climb out!"
770 =192:PRINT #0," You are at the top of a rough,poorlycut,stone staircase.There is a       passage to your east.":d=1:e=1:IF RND(5)=1THEN mo=RND(3)
780 =292:PRINT #0," You are at the bottom of a staircaseThe passage leads north.It looks verydark and the air smells of rotting   flesh.":n=1:u=1:IF RND(2)=1 THEN mo=RND(3)
790 =302:PRINT #0," You are in a small windy passage    which curves south and east.":s=1:e=1:IF RND(2)=1THEN mo=RND(4)
800 =303:PRINT #0," You are in a narrow passage which   goes west and north.":n=1:w=1:IF RND(2)=1 THEN mo=2
810 =313:PRINT #0,ns$:n=1:s=1
820 =323:PRINT #0,ns$:n=1:s=1
830 =333:PRINT #0,ns$:n=1:s=1:IF RND(3)=1 THEN mo=RND(8)
840 =343:PRINT #0," You are at the meeting place of     eight identical passages.":n=1:s=1:e=1:w=1:se=1:sw=1:ne=1:nw=1:IF RND(5)=1 THEN mo=RND(10)
850 =332:PRINT #0," The passage bends in a gentle curve.You may go north-east or west.":ne=1:w=1:IF RND(5)=1 THEN mo=4
860 =331:PRINT #0," You are at what seems to be a dead  end.The only passage goes east.":e=1:w=1:IF RND(2)=1 THEN mo=RND(2)
870 =341:PRINT#0," You are at a dead end.A passage     leads east.":e=1:w=1:IF RND(2)=1 THEN mo=RND(2)
880 =330:PRINT #0," The passage goes north.To your east you can see a large iron door.It     appears to be stuck fast.On every    part of every wall are huge gems.Theymust be worth a fortune!There are    piles of bones everywhere.":n=1:mo=RND(2)
890 =340:PRINT #0," The way east is blocked by a huge   iron door.The only passage leads     south.There are enormous gems        embedded in the rock.The floor is    thick with skeletons of every sort.":s=1:mo=1
900 =342:PRINT #0," The passage leads west and east.":w=1:e=1:IF RND(4)=1THEN mo=RND(2)
910 =352:PRINT #0," The path goes north and south-east. To the north the path looks as thoughits getting narrower.":n=1:se=1:IF RND(3)=1THEN mo=RND(4)
920 =362:PRINT #0," The path is very narrow here.You maygo south or east.":e=1:s=1:IF RND(6)=1THEN mo=RND(3)
930 =363:PRINT #0," The passage curves here.To the souththe path is broader but to the west  it looks like a tight squeeze!":w=1:s=1
940 =353:PRINT #0,ns$:n=1:s=1:IF RND(4)=1 THEN mo=RND(3)
950 =334:PRINT #0," You are in a straight,man-made      passage which heads north-west and   south-east.":se=1:nw=1:IF RND(3)=1 THEN mo=RND(6)
960 =344:PRINT #0," You are in an east-west passage.To  your east you can see a large iron   door,which at present is raised.It   might be risky but you could try     getting past.":e=1:w=1:IF RND(2)=1THEN mo=RND(5)
970 =345:PRINT #0," You are in a fairly large room with an unusually high ceiling.There is anenormous table at its centre.The top is level with your shoulders.To your west you can see a large iron door.  You may go south.":s=1
980 IF RND(3)=1 THEN mo=RND(3)
990 IF og=0 THEN mo=2
1000 =335:PRINT #0,ns$:n=1:s=1:IF RND(4)=1THEN mo=2
1010 =325:PRINT #0," You are at a T junction.A passage   leads north.The main passage goes in a north-west/south-east direction.":n=1:nw=1:se=1:IF RND(5)=1THEN mo=RND(3)
1020 =316:PRINT #0," You are at the western end of a     large hall.There is a passage leadingnorth-west and to your east is a     throne.On it is seated a man who is  wearing a cloak.He looks very        powerful.":nw=1:e=1
1030 IF RND(3)=1 THEN mo=RND(3)
1040 IF nn=0 THEN mo=4
1050 =317:PRINT #0," You are at the eastern end of the   great hall.On the throne sits a man, he is ghost like in appearance,      although his voice is real enough,it booms:'What do you want?'.To your    east you can see a massive gem at thecentre of a small room!":e=1:w=1:IF RND(3)=1 THEN mo=RND(2)
1060 =318:PRINT #0," You are in a small square room.The  only exit is east.The gem is within  your grasp....Suddenly you are cut toshreads as 10 spears shoot out from  every wall!":PAUSE:RUN
1070 =365:PRINT #0," You are in a wide,well kept passage going north-east and south-west.":ne=1:sw=1:IF RND(2)=1THEN mo=RND(9)
1080 =354:PRINT #0," You are in a wide,well kept passage going north-east and south-west.":ne=1:sw=1:IF RND(2)=1THEN mo=RND(7)
1090 =376:PRINT #0," You are at the end of a north-east  passage.In front of you are two doors,one leads north,the other north-east":n=1:sw=1:ne=1:IF RND(3)=1THEN mo=RND(3)
1100 =386:PRINT #0," You walk forward,and realize that   there is no floor,but too late....Youare impaled on a stake,your screams  echo through the dungeon...!":PAUSE:RUN
1110 =387:PRINT #0," You are in a straight passage which goes north-east and south-west.There are strange writings on the wall.You can't read them.":ne=1:sw=1:IF RND(2)=1THEN mo=RND(4)
1120 =398:PRINT #0," The passage bends north here,but    goes south-west also.The strange     writings cover every wall but one.":n=1:sw=1:e=1:IF RND(2)=1THEN mo=RND(5)
1130 =399:PRINT #0," The passage widens but finishes.Youronly means of escape being to the    west.In the centre of the room is an open coffin!":w=1:e=1
1140 IF va=0 THEN mo=1
1150 =400:PRINT #0," You are in a narrow passage.To your west you can see light,to your north is a great hall.":w=1:n=1:IF RND(2)=1THEN mo=RND(4)
1160 =410:PRINT #0," You are in a great hall.A passage   goes south.To your east is a golden  throne.On both sides of it are huge  piles of gold and gems!";:s=1:e=1
1170 IF tt=0 THEN mo=RND(8)
1180 IF wi=0THEN PRINT #0,"You can see anevil looking wizard perched on the   throne."
1190 IF wi=1THEN PRINT#0
1200 =411:PRINT #0," You are at the eastern end of the   great hall,by the throne.You can see clearly the great treasures that     await you!";:w=1
1210 IF wi=0 THEN PRINT #0,"The wizard walks towards   you!":mo=1
1220 IF wi=1 THEN PRINT #0;"The treasure is yours!You  have gained riches surpassing your   wildest imaginings!Well done.":PAUSE:RUN
1230 =408:PRINT #0,ns$:n=1:s=1:IF RND(3)=1THEN mo=RND(3)
1240 =418:PRINT #0," You are at the southern end of a    large room.There appears to be some  kind of shrine to the north.You may  leave via the southern exit.":s=1:n=1:IF RND(2)=1THEN mo=RND(4)
1250 =428:PRINT #0," You are next to the shrine at the   northern end of the hall.You can see a passage south.";:s=1:IF RND(2)=1 THEN mo=RND(4)
1260 IF tb=0 THEN PRINT #0;"The tomb looks heavy but you might be able to remove the  lid."
1270 IF tb=1 THEN PRINT #0;"The tomb is empty,it has already been plundered!"
1280 END SELect
1290 IF oxo>0 THEN PRINT #0,"You are invisible!":oxo=oxo-1
1300 test=0
1310 PRINT #0;"You"!"can"!"see:"!
1320 IF mo>0 THEN chomon:desmon:test=1
1330 IF po=10 THEN GO TO 1380
1340 IF tre=1THEN test=1:GO TO 1370
1350 IF RND(10)=1 THEN tre=1:test=1
1360 IF tre=1 AND po<>187AND po<>197THEN gold=RND(2 TO 30):gold=gold*gold
1370 IF gold>0 THEN PRINT #0,!gold!"gold"!"pieces!"!
1380 FOR i=1TO 11
1390 IF b(i)=po THEN PRINT #0,!b$(i);","!:test=1
1400 NEXT i
1410 IF test=0 THEN PRINT #0,!"nothing"!"of"!"any"!"value."!
1420 crap=0
1430 IF mt=1AND ma=0 THEN mancon
1440 IF crap=1THEN CLS#0:GO TO 270
1450 IF mo>0THEN GO TO 3090
1460 IF po=10 OR po=187 OR po=197 THEN GO TO 1480
1470 IF RND(5)=2AND jusmov=1THEN mo=RND(1 TO 5):CLS#0:gold=0:tre=0:GO TO 310
1480 IF INT(xe/1000)>le-1 THEN oo=INT(xe/1000)-le+1:FOR t=1 TO oo:le=le+1:god=RND(5 TO 10):lp=lp+god:max=max+god:NEXT t:PRINT"You have gone up a level!":STAT
1490 PRINT"What do you want to do now?"
1500 aa=2:jusmov=1
1510 INK 7:INPUT d$
1511 IF d$<>''
1512 FOR II=1 TO LEN(d$):IF d$(II)>='A':IF d$(II)<='Z':d$(II)=CHR$(CODE('D$(II)')+32)
1513 END IF
1520 INK 5
1530 IF "go"INSTR d$ AND("north-west"INSTR d$ OR"north-east"INSTR d$ OR "south-west"INSTR d$ OR "south-east"INSTR d$)THEN d$=d$(4)& d$(10)
1540 IF "go"INSTR d$ AND("north"INSTR d$ OR "south"INSTR d$ OR "east"INSTR d$ OR "west"INSTR d$ OR "up"INSTR d$ OR "down"INSTR d$)THEN d$=d$(4)
1550 IF d$=="n"OR d$=="s"OR d$=="e"OR d$=="w"OR d$=="u"OR d$=="d"OR d$=="ne"OR d$=="nw"OR d$=="se"OR d$=="sw" THEN shift
1560 IF d$=="save"THEN aa=0:PRINT"Are you sure(Y or N)?":cba$=INKEY$(-1):IF cba$="y"THEN save_:aa=3
1570 IF d$=="load"THEN aa=0:PRINT"Are you sure(Y or N)?":cba$=INKEY$(-1):IF cba$="y"THEN load_:aa=3
1580 IF d$=="restore"THEN aa=0:PRINT"Are you sure(Y or N)?":cba$=INKEY$(-1):IF cba$=="y"THEN RUN
1590 IF d$=="quit"THEN aa=0:PRINT"Are you sure(Y or N)?":cba$=INKEY$(-1):IF cba$=="y"THEN STOP
1600 IF d$=="help" THEN PRINT"Sorry no help allowed,its easy enoughas it is!":aa=0
1610 IF "take"INSTR d$ OR "get"INSTR d$ THEN nab
1620 IF "drop"INSTR d$ OR "leave"INSTR d$ THEN drop
1630 IF "sit"INSTR d$ THEN sit
1640 IF "use"INSTR d$ THEN use
1650 IF "examine"INSTR d$ OR "search"INSTR d$ THEN exam
1660 IF "drink"INSTR d$ THEN drink
1670 IF "tred"INSTR d$ THEN tred
1680 IF "untie"INSTR d$ THEN untie
1690 IF d$=="look"OR d$=="l"THEN aa=3
1700 IF "say"INSTR d$ THEN say:GO TO 1770
1710 IF "press"INSTR d$OR"push"INSTR d$THEN button
1720 IF "ram"INSTR d$ OR"bust"INSTR d$ OR"brake"INSTR d$ OR"smash"INSTR d$ OR"charge"INSTR d$THEN smash
1730 IF "open"INSTR d$THEN ope
1740 IF "unlock"INSTR d$THEN unlock
1750 IF "kill" INSTR d$ THEN kill
1760 IF "lift"INSTR d$ OR "force"INSTR d$ THEN tomb
1770 IF aa=3 THEN CLS#0:PRINT"O.K.":GO TO 270
1780 IF aa=1 THEN PRINT"O.K."
1790 IF aa=2 THEN PRINT"I'm sorry,I don't understand."
1800 GO TO 1460
1810 DEFine PROCedure shift
1820 xe=xe+3
1830 IF d$=="n"AND po=193THEN po=203:PRINT"You go tumbling into a pit!":lp=lp-3:aa=3:waes=0:RETurn
1840 IF d$=="e"AND po=140AND pp=0THEN PRINT"The door appears to be jammed!":aa=0:RETurn
1850 IF d$=="w" AND (po=331OR po=341)THEN po=po-1:PRINT"As you walk west,suddenly from behindyou a massive iron door thunders down":aa=3:waes=0:RETurn
1860 IF (d$=="n"AND po=141)OR(d$=="e"AND po=398)OR(d$=="e"AND po=399)OR(d$=="n"AND po=170)THEN PRINT"You walk straight through the wall!"
1870 IF po=376 AND d$=="n"THEN PRINT"You walk through the door and tumble into nothingness...!":po=386:aa=3:waes=0:RETurn
1880 IF po=344 AND d$=="e"THEN PRINT"Suddenly from behind you a huge iron door slams shut!":po=345:aa=3:waes=0:RETurn
1890 IF (po=187 AND d$=="n")OR(po=197 AND d$=="s")THEN PRINT"Suddenly,as you walk across the      squares your life comes to a painful end!":PAUSE:RUN
1900 IF po=187 AND d$=="n"THEN th=0:waes=0
1910 IF po=176 AND d$=="e"THEN th=0:waes=0
1920 IF d$=="n"AND n=1THEN po=po+10:aa=3:waes=2:RETurn
1930 IF d$=="s"AND s=1THEN po=po-10:aa=3:waes=1:RETurn
1940 IF d$="e"AND e=1THEN po=po+1:aa=3:waes=4:RETurn
1950 IF d$="w"AND w=1THEN po=po-1:aa=3:waes=3:RETurn
1960 IF d$="u"AND u=1THEN po=po-100:aa=3:waes=6:RETurn
1970 IF d$="d"AND d=1THEN po=po+100:aa=3:waes=5:RETurn
1980 IF d$="nw"AND nw=1THEN po=po+9:aa=3:waes=8:RETurn
1990 IF d$="ne"AND ne=1THEN po=po+11:aa=3:waes=10:RETurn
2000 IF d$="sw"AND sw=1THEN po=po-11:aa=3:waes=7:RETurn
2010 IF d$="se"AND se=1THEN po=po-9:aa=3:waes=9:RETurn
2020 PRINT"You can't go that way!":xe=xe-3:aa=0
2025 END DEFine
2030 DEFine PROCedure nab
2040 IF "gold"INSTR d$ AND tre=1THEN gp=gp+gold:xe=xe+gold:aa=1:tre=0:STAT:RETurn
2050 IF "box"INSTR d$ THEN GO TO 2910
2060 FOR i=1TO 11
2070 IF b(i)=po AND b$(i)INSTR d$THEN b(i)=0:i=11:aa=3:STAT:RETurn
2080 NEXT i
2090 aa=2
2095 END DEFine
2100 DEFine PROCedure drop
2110 IF "gold"INSTR d$ AND gp>0THEN PRINT"Giving away gold now,are we?!!":aa=0:RETurn
2120 FOR i=1TO 11
2130 IF b(i)=0 AND b$(i)INSTR d$THEN b(i)=po:i=11:aa=3:STAT:RETurn
2140 NEXT i
2150 aa=2
2155 END DEFine
2160 DEFine PROCedure sit
2170 IF NOT("down"INSTR d$OR "throne"INSTR d$):aa=2:RETurn
2190 IF po=176OR po=187THEN th=1:PRINT"You sit on the hard,rock throne.":aa=0:RETurn
2200 IF po=411THEN PRINT"You sit on the throne,you feel very  powerful.":aa=0:RETurn
2210 IF po=317THEN PRINT"You are sitting in the wizards image!":aa=0:RETurn
2220 aa=2
2225 END DEFine
2230 DEFine PROCedure use
2240 IF "key"INSTR d$THEN unlock:RETurn
2250 IF NOT("gem"INSTR d$):aa=2:RETurn
2270 IF b(12)<>0THEN PRINT"You have no gem!":aa=0:RETurn
2280 IF po<>176THEN PRINT"You can find no use for the gem here.":aa=0:RETurn
2290 IF th=0THEN PRINT"You insert the gem into its rightful position but nothing happens!":aa=0:RETurn
2300 PRINT"As soon as you place the gem in the  notch it turns to powder!Suddenly youare in a different room!":po=187:b(12)=-1:aa=3:xe=xe+514:STAT
2305 END DEFine
2310 DEFine PROCedure exam
2320 IF "orc"INSTR d$AND po=154AND oc=1AND b(2)=-1THEN b(2)=0:PRINT"You find a large and rather well madekey in the scum's pocket!":STAT:xe=xe+105:STAT:aa=0:RETurn
2330 IF "orc"INSTR d$THEN PRINT"You find nothing of any value.":aa=0:RETurn
2340 IF "man"INSTR d$AND po=154AND mt=0THEN PRINT"The man looks at you angerily,'I havenothing you would want!'You search   him and find a few useless herbs.":aa=0:RETurn
2350 IF "man"INSTR d$AND po=mp THEN PRINT"The man is shocked,'Unhand me you    scoundrel,before I melt your head!'":aa=0:RETurn
2360 IF "wall"INSTR d$AND (po=141OR po=170)THEN PRINT"You find a hidden passage leading    north!":aa=0:xe=xe+35:RETurn
2370 IF "wall"INSTR d$AND(po=341OR po=331)THEN PRINT"You find a hidden passage leading    west!":aa=0:xe=xe+20:RETurn
2380 IF "wall"INSTR d$AND(po=398OR po=399)THEN PRINT"You find a small gap in the east wallIts large enough to crawl through!":aa=0:xe=xe+22:RETurn
2390 IF "throne"INSTR d$AND po=187THEN PRINT"To your amazement you find a hidden  button!":aa=0:xe=xe+75:RETurn
2400 IF "throne"INSTR d$AND po=176THEN PRINT"You find a small round indentation inthe throne!":aa=0:xe=xe+28:RETurn
2410 IF "throne"INSTR d$AND (po=317OR po=411)THEN PRINT"You find nothing of any significance.":aa=0:RETurn
2420 IF "bones"INSTR d$AND po=194AND b(12)=-1THEN PRINT"To your amazement you find a gem!":aa=0:b(12)=0:xe=xe+275:STAT:RETurn
2430 IF "bones"INSTR d$THEN PRINT"You find nothing of any great value.":aa=0:RETurn
2440 IF "tomb"INSTR d$AND po=428AND tb=0THEN PRINT"You notice a crack all around the    tomb,as though the lid might come    off....":aa=0:RETurn
2450 IF "wall"INSTR d$AND po=169 THEN PRINT"You find a keyhole in the wall!":aa=0:xe=xe+30:RETurn
2460 aa=0:PRINT"You find nothing of any significance."
2465 END DEFine
2470 DEFine PROCedure drink
2475 IF "invis"INSTR d$AND b(6)=0THEN aa=1:b(6)=-1:xe=xe+85:STAT:oxo=5:RETurn
2480 IF "heal"INSTR d$AND b(5)=0THEN aa=0:PRINT"You feel a warm glow spread through  you.":b(5)=-1:xe=xe+150:lp=lp+RND(20):IF lp>max THEN lp=max
2490 IF "heal"INSTR d$AND b(5)=-1THEN STAT:RETurn
2500 IF "invis"INSTR d$OR "heal"INSTR d$THEN PRINT"You aren't carrying it!":aa=0:RETurn
2505 aa=2
2510 END DEFine
2520 DEFine PROCedure tred
2530 IF NOT("blue"INSTR d$OR "red"INSTR d$OR"green"INSTR d$):aa=2:RETurn
2550 IF ("blue"INSTR d$OR " red"INSTR d$)AND(po=187OR po=197)THEN PRINT"As soon as you touch one of the      squares the floor opens up beneath   you,and you fall into a pit of moltenrock!":PAUSE:RUN
2560 IF "green"INSTR d$AND(po=187OR po=197)THEN PRINT"You manage to cross safely.":aa=3:hyt=192-po:hyt=hyt*2:po=po+hyt:xe=xe+20:aa=3:RETurn
2570 aa=2
2575 END DEFine
2580 DEFine PROCedure untie
2590 IF "rope"INSTR d$OR"man"INSTR d$AND po=154AND mt=0THEN mt=1:aa=0:xe=xe+46:PRINT"He says calmly:'I am very grateful   for all that you've done,I look      forward to repaying my debt.'":RETurn
2600 IF mt=1 THEN PRINT"You've already untied him!":aa=0:RETurn
2610 aa=2
2615 END DEFine
2620 DEFine PROCedure say
2630 IF "who"INSTR d$ AND po=mp THEN PRINT"My name is Alric,I am a wizard of    some repute.":aa=0:RETurn
2640 IF "why"INSTR d$ AND po=mp THEN PRINT"I was caught by the evil Orc whom youso kindly slew,whilst on my quest forthe evil wizard who runs this foul   place.";:PAUSE:PRINT"He is the cause of all evil    that plagues this once fair land.It  was on this same quest that my fatherwas most cruelly slain.":aa=0:RETurn
2650 IF po<>mp THEN PRINT"The man is not here!":aa=0:RETurn
2660 IF "lift"INSTR d$AND"lid"INSTR d$AND po=mp AND tb=0AND po=428THEN tb=1:PRINT"He says'Yes,of course!'Together you  just manage to lift the lid.":aa=3:xe=xe+260:RETurn
2670 IF "lift"INSTR d$AND"lid"INSTR d$AND po=mp THEN PRINT"He booms'The lid is already off!Are  you insane?!":aa=0:RETurn
2680 aa=2
2685 END DEFine
2690 DEFine PROCedure kill
2700 IF "man"INSTR d$AND po=mp AND mt=1THEN PRINT"The wizard sees the intent in your   eyes.Suddenly huge jets of flame     shoot from his out stretched arms.Youare burnt to a crisp!":PAUSE:RUN
2710 IF "man"INSTR d$AND po<>mp THEN PRINT"You can't even see him!":aa=0:RETurn
2720 IF "man"INSTR d$AND po=mp THEN PRINT"His dying screams seem to last       forever..":aa=0:mt=0:mp=0:ma=1:RETurn
2730 aa=2
2735 END DEFine
2740 DEFine PROCedure tomb
2750 IF "lid"INSTR d$AND po=428AND tb=0THEN PRINT"The tomb won't budge an inch!":aa=0:RETurn
2760 IF "lid"INSTR d$AND po=428THEN PRINT"The lid is already off!":aRET
2770 aa=2
2775 END DEFine
2780 DEFine PROCedure smash
2790 IF "door"INSTR d$AND po=140AND pp=0THEN PRINT"With alot of effort you manage to    bash the door down!":pp=1:aa=3:xe=xe+15:RETurn
2800 IF "door"INSTR d$AND po=140THEN PRINT"The door is already unlocked!":aa=0:RETurn
2810 IF "door"INSTR d$AND(po=169OR po=330OR po=331)THEN PRINT"However hard you try,you can't bash  the door down.":aa=0:RETurn
2820 IF "lock"INSTR d$AND(po=197OR po=169)THEN PRINT"However hard you try,you cannot breakthe lock!":aa=0:RETurn
2830 IF "chest"INSTR d$AND po=197THEN PRINT"You have no luck,the chest is well-  made.":aa=0:RETurn
2840 aa=2
2845 END DEFine
2850 DEFine PROCedure ope
2860 IF "door"INSTR d$AND po=140AND pp=1THEN PRINT"The western door is already open!The eastern door hes a gaping hole in it!":aa=0:RETurn
2870 IF "door"INSTR d$AND po=140THEN PRINT"The western door is open,the eastern is jammed!":aa=0:RETurn
2880 IF "chest"INSTR d$AND po=197AND ch=1THEN PRINT"You lift the heavy lid and gasp as   you see the magnificent golden,magic,armour and sword!You throw down your old weapons and try on the new.":b(3)=-1:b(4)=-1:b(7)=0:b(8)=0:xe=xe+4500:ch=2:aa=0:RETurn
2890 IF "chest"INSTR d$AND po=197AND ch=2THEN PRINT"The chest is already open,what more  do you want!":aa=0:RETurn
2900 IF "chest"INSTR d$AND po=197THEN PRINT"The chest is locked!":aa=0:RETurn
2910 IF "box"INSTR d$AND po=139AND bt=0THEN PRINT"You pocket 87 gold pieces and a largerusty key!The box has nothing else tooffer.":aa=0:xe=xe+115:gp=gp+87:bt=1:b(1)=0:STAT:RETurn
2920 IF "box"INSTR d$AND po=139THEN PRINT"The box is empty!":aa=0:RETurn
2930 IF "door"INSTR d$AND(po=340OR po=330)THEN PRINT"The door is as solid as rock.You are doomed to stay here for the rest of  your life!":PAUSE:RUN
2940 IF "lid"INSTR d$AND tb=0THEN PRINT"You are not strong enough-weed!":aa=0:RETurn
2950 aa=2
2955 END DEFine
2960 DEFine PROCedure unlock
2970 IF ("door"INSTR d$OR "key"INSTR d$)AND po=169AND id=0AND b(1)=0THEN id=1:aa=3:PRINT"You here a low rumbling sound and thedoor begins to rise...":xe=xe+75:RETurn
2980 IF ("door"INSTR d$OR "key"INSTR d$)AND po=169AND id=0AND b(2)=0THEN aa=0:PRINT"No matter how you try it,it just willnot fit!":RETurn
2990 IF ("door"INSTR d$OR "key"INSTR d$)AND po=169AND id=0THEN PRINT"You have no key!":aa=0:RETurn
3000 IF ("chest"INSTR d$OR "key"INSTR d$)AND po=197AND ch=0AND b(2)=0THEN ch=1:aa=0:PRINT"You unlock the chest!":xe=xe+50:RETurn
3010 IF ("chest"INSTR d$OR "key"INSTR d$)AND po=197AND ch=0AND b(1)=0THEN PRINT"The key does not fit!":aa=0:RETurn
3020 IF ("chest"INSTR d$OR "key"INSTR d$)AND po=197AND ch=0THEN PRINT"You have no key!":aa=0:RETurn
3030 IF ("chest"INSTR d$OR "key"INSTR d$)AND po=197THEN PRINT"The chest is already open!":aa=0:RETurn
3040 aa=2
3045 END DEFine
3050 DEFine PROCedure button
3060 IF "button"INSTR d$AND po=187THEN po=176:xe=xe+125:aa=3:PRINT"Your brain swirls for a second and   then all is normal...":RETurn
3070 IF "button"INSTR d$THEN PRINT"Try it somewhere else!!":aa=0:RETurn
3080 aa=2
3085 END DEFine
3090 STAT:PRINT"F(ight),P(arley),R(un),U(se) potions?"
3100 x$=INKEY$(100)
3110 IF x$="u"THEN GO TO 4130
3120 IF x$="r"THEN GO TO 4000
3130 IF x$="p"THEN GO TO 3950
3140 IF x$<>"f"THEN aw=0:GO TO 3660
3150 v1=0:v2=0:v3=0:v4=0:v5=0
3160 IF b(3)=0THEN v1=1
3170 IF b(11)=0THEN v2=1
3180 IF b(9)=0THEN v3=1
3190 IF b(7)=0THEN v5=1
3200 IF b(10)=0THEN v4=1
3210 cg=0
3220 IF v1+v2+v3+v4+v5=0THEN PRINT"You must use your hands!":aw=5:cg=-1:GO TO 3430
3230 IF v1+v2+v3+v4+v5<>1THEN GO TO 3300
3240 PRINT"You must use your ";
3250 IF v1=1 THEN PRINT;b$(3):aw=10:cg=3:GO TO 3430
3260 IF v2=1 THEN PRINT;b$(11):aw=8:cg=11:GO TO 3430
3270 IF v3=1 THEN PRINT;b$(9):aw=6:cg=9:GO TO 3430
3280 IF v4=1 THEN PRINT;b$(10):aw=3:cg=10:GO TO 3430
3290 IF v5=1 THEN PRINT;b$(7):aw=20:cg=7:GO TO 3430
3300 PRINT"Choose your weapon:"
3310 IF v1=1THEN PRINT"Enter '1' for your ";b$(3)
3320 IF v2=1THEN PRINT"Enter '2' for your ";b$(11)
3330 IF v3=1THEN PRINT"Enter '3' for your ";b$(9)
3340 IF v4=1THEN PRINT"Enter '4' for your ";b$(10)
3350 IF v5=1THEN PRINT"Enter '5' for your ";b$(7)
3360 j$=INKEY$(100)
3370 IF j$="1"AND v1=1THEN aw=10:cg=3:GO TO 3430
3380 IF j$="2"AND v2=1THEN aw=8:cg=11:GO TO 3430
3390 IF j$="3"AND v3=1THEN aw=6:cg=9:GO TO 3430
3400 IF j$="4"AND v4=1THEN aw=3:cg=10:GO TO 3430
3410 IF j$="5"AND v5=1THEN aw=20:cg=7:GO TO 3430
3420 GO TO 3660
3430 IF po=399 AND va=0 AND cg=10 THEN aw=aw*4
3440 IF cg=9 AND mo=9 THEN aw=aw*2
3450 IF po=399 AND va=0 AND cg<>10 THEN aw=0
3460 aw=aw+(2*oxo):IF aw+2>RND(1 TO 1.5*mhp)THEN
3470 IF po=399 AND va=0 AND cg=10 THEN PRINT"You plunge the stake deep into the   vampires heart.It is dead!":va=1:CLS #0:xe=xe+340:STAT:GO TO 290
3480 PRINT"You hit...";:PAUSE 100:PRINT"doing ";
3490 hq=RND(1TO aw):PRINT hq;" damage.":mh(yma)=mh(yma)-hq
3500 PRINT"The monster staggers...";:PAUSE 100:IF mh(yma)>0THEN PRINT"and recovers."
3510 IF mh(yma)<=0THEN PRINT"and dies.":yma=yma+1:xe=xe+mhp*3+RND(1TO 10):STAT
3520 IF yma<=mo AND mp=po AND mt=1THEN PRINT"Alric attacks,and does...";:dam=RND(1TO 8):PRINT dam;" damage!":mh(yma)=mh(yma)-dam:IF mh(yma)<=0THEN PRINT"The poor creature has been fried!":yma=yma+1:PAUSE 50
3530 IF po=139 THEN od=1
3540 IF po=411 THEN wi=1
3550 IF po=153 THEN os=1
3560 IF po=194 THEN ra=1
3570 IF po=154 THEN oc=1
3580 IF po=177 THEN sk=1
3590 IF po=345 THEN og=1
3600 IF po=316 THEN nn=1
3610 IF po=410 THEN tt=1
3620 IF yma>mo THEN CLS#0:GO TO 290
3630 GO TO 3660
3640 END IF
3650 PRINT"You miss the foul fiend.":PAUSE 100
3660 tj=0
3670 FOR i=yma TO mo
3680 IF mh(i)>0THEN tj=tj+1
3690 NEXT i
3700 IF tj=1 THEN
3710 mq=0:IF b(4)=0 THEN mq=3
3720 PRINT"The monster attacks...";:PAUSE 100
3730 IF b(8)=0THEN mq=5
3740 mq=mq+oxo:IF RND(0 TO mq)>2THEN PRINT"and misses.":GO TO 3090
3750 hq=RND(1TO 5+mhp)
3760 IF va=0 AND po=399 THEN PRINT"The vampire    sinks its teeth into your throat!You fall unconcious as he tucks into his evening meal...":PAUSE:RUN
3770 PRINT"doing ";hq;" damage"
3780 lp=lp-hq
3790 IF lp<=0THEN PRINT"Your stomach is ripped open and you  die a painful death!Another victim ofthe lost tomb of ORNAK.":PAUSE:RUN
3800 GO TO 3090
3810 END IF
3820 PRINT"The monsters attack...";:gh=3:PAUSE 100:IF b(4)=0 THEN gh=5
3830 IF b(8)=0 THEN gh=8
3840 pq=0
3850 FOR i=1 TO tj
3860 IF RND(0 TO 9)>gh+oxo THEN pq=pq+RND(1 TO 8)
3870 NEXT i
3880 IF mp=po THEN pq=INT(pq/2)
3890 PRINT"doing ";pq;" damage"
3900 IF mp=po AND mt=1THEN PRINT"Alric takes ";INT(pq/3);" damage.":mah=mah-INT(pq/3):IF mah<=0 THEN ma=1:mp=0:PRINT"Alric collapses,he is dead!"
3910 PAUSE 100
3920 lp=lp-pq
3930 IF lp<=0THEN PRINT"Your skull is smashed and your guts  are hanging out!I think you're dead!":PAUSE:RUN
3940 GO TO 3090
3950 PRINT"You sheath your weapon and speak     words of peace...";:PAUSE 100
3960 IF yma=11THEN PRINT"The wizard bellows, 'I have no mercy for your kind!'":PAUSE 100:GO TO 3660
3970 IF yma=14THEN PRINT"Alric smiles and    says'I am glad you have seen wisdom!'":GO TO 290
3980 PRINT"there seems to be noresponse!":PAUSE 100
3990 GO TO 3660
4000 IF waes=0THEN PRINT"You must stand and fight!":PAUSE 50:GO TO 3660
4010 PRINT"You flee to the...";:PAUSE 50
4020 IF waes=1THEN PRINT"north.":d$="n"
4030 IF waes=2THEN PRINT"south.":d$="s"
4040 IF waes=3THEN PRINT"east.":d$="e"
4050 IF waes=4THEN PRINT"west.":d$="w"
4060 IF waes=5THEN PRINT"up.":d$="u"
4070 IF waes=6THEN PRINT"down.":d$="d"
4080 IF waes=7THEN PRINT"north-east.":d$="ne"
4090 IF waes=8THEN PRINT"south-east.":d$="se"
4100 IF waes=9THEN PRINT"north-west.":d$="nw"
4110 IF waes=10THEN PRINT"south-west.":d$="sw"
4120 shift:CLS#0:GO TO 270
4130 IF b(5)<>0AND b(6)<>0THEN PRINT"You have no potions!":PAUSE 100:GO TO 3660
4140 IF b(5)=0 AND b(6)<>0THEN PRINT"You drink it!You feel a warm glow    spread through you!":b(5)=-1:lp=max:xe=xe+180:STAT:PAUSE 100:GO TO 3660
4150 IF b(6)=0 AND b(5)<>0THEN PRINT"You drink it!You are now very nearly invisible!":b(6)=-1:xe=xe+180:STAT:oxo=5:PAUSE 100:GO TO 3660
4160 PRINT"Choose a potion..."
4170 PRINT"Press (1) for healing."
4180 PRINT"Press (2) for invisibility."
4190 resp$=INKEY$(100)
4200 IF resp$=""OR(resp$<>"1"AND resp$<>"2")THEN GO TO 3660
4210 IF resp$="1"THEN PRINT"You feel a warm glow spread through  -out your entire body!":lp=max:b(5)=-1:xe=xe+180:STAT:GO TO 3660
4220 IF resp$="2"THEN PRINT"You become very nearly invisible!":oxo=5:b(6)=-1:xe=xe+180:STAT:PAUSE 100:GO TO 3660
4230 DEFine PROCedure STAT
4235 CLS#2:s1$="'"& na$ & "' Life points="& lp
4240 s2$="Level="& le
4250 s3$="Gold pieces="& gp
4260 s4$="Experience="& xe
4270 PRINT #2,!s1$!s2$!s3$!s4$!
4280 PRINT #2,!" You are carrying:"!:abc=0
4290 FOR i=1TO 12
4300 IF b(i)=0 THEN PRINT #2,!b$(i)!:abc=1
4310 NEXT i
4320 IF abc=0 THEN PRINT#2;"Nothing."
4330 END DEFine
4340 DEFine PROCedure initialize
4350 ns$=" You are in a narrow,winding north-  south passage.Occasionally you here  some very eerie noises.There is a    cold breeze blowing on your face."
4360 DIM a$(13,10),a(13)
4370 DIM b$(12,21),b(12)
4380 DIM mh(13)
4390 FOR i=1TO 13
4400 READ a$(i):READ a(i)
4410 NEXT i
4420 FOR i=1TO 12
4430 READ b$(i),b(i)
4440 NEXT i
4450 n=0:s=0:e=0:w=0:u=0:d=0:nw=0:ne=0:se=0:sw=0
4460 END DEFine
4470 DEFine PROCedure chomon
4480 IF po=153 AND os=0THEN mc=1:mhp=RND(1TO a(1)):mh(1)=mhp:yma=1:RETurn
4490 IF po=154 AND oc=0THEN mc=1:mhp=RND(1TO a(1)*2):mh(1)=mhp:yma=1:RETurn
4500 IF po=139 AND od=0THEN mc=1:mhp=RND(1TO a(1)/2):mh(1)=mhp:yma=1:RETurn
4510 IF po=194 AND ra=0THEN mc=5:mhp=RND(1TO a(5)):FOR i=1TO mo:mh(i)=mhp:NEXT i:yma=1:RETurn
4520 IF po=399 AND va=0THEN mc=13:mhp=RND(1TO a(13)):mh(1)=mhp:yma=1:RETurn
4530 IF po=411 AND wi=0THEN mc=11:mhp=RND(1TO a(11)):mh(1)=mhp:yma=1:RETurn
4540 IF po=345 AND og=0 THEN mc=12:mhp=RND(1TO a(12)):FOR i=1TO mo:mh(i)=mhp:NEXT i:yma=1:RETurn
4550 IF po=410 THEN mc=10:mhp=RND(1TO a(10)):FOR i=1TO mo:mh(i)=mhp:NEXT i:yma=1:RETurn
4560 mc=RND(1 TO 10)
4570 mhp=RND(1TO a(mc))
4580 FOR i=1TO mo
4590 mh(i)=mhp
4600 NEXT i
4610 yma=1
4620 END DEFine
4630 DEFine PROCedure desmon
4640 PM$=""
4650 IF mo=1 THEN PM$=PM$ &"a"
4660 IF mo=1 AND (mc=1OR mc=12)THEN PM$=PM$ &"n"
4670 IF mo>1 THEN PM$=PM$ & mo
4680 PM$=PM$ & " "& a$(mc)
4690 IF mo>1 THEN PM$=PM$ &"s"
4700 PM$=PM$ & "."
4710 PRINT #0,!PM$!
4720 END DEFine
4730 DEFine PROCedure mancon
4740 IF mp=po THEN PRINT"Alric is standing at your side.":RETurn
4750 IF mp=176 AND(po=187OR po=197)THEN PRINT"There is no sign of Alric.":RETurn
4760 IF po=203 AND mp=193 THEN PAUSE 600:PRINT"A little head pops over the rim of   the pit.It is Alric,he chuckles      quietly.'It  looks as though I can   repay you!'He mumbles for a minute ortwo,you become impatient.Suddenly youstart to float up and out of the pit!":po=193:crap=1:PAUSE 600:RETurn
4770 IF mp=po-10AND n1=1THEN mp=po:PRINT"Alric appears from the south.":RETurn
4780 IF mp=po-1AND e1=1THEN mp=po:PRINT"Alric appears from the east.":RETurn
4790 IF mp=po-11AND ne1=1THEN mp=po:PRINT"ALRIC walks up to you.":RETurn
4800 IF mp=po-100AND d1=1THEN mp=po:PRINT"ALRIC joins you at the bottom step.":RETurn
4810 IF mp=po-9AND nw1=1THEN mp=po:PRINT"Alric rejoins you.":RETurn
4820 IF mp=po+1AND w1=1THEN mp=po:PRINT"Alric joins you from the east.":RETurn
4830 IF mp=po+10AND s1=1THEN mp=po:PRINT"Alric appears.":RETurn
4840 IF mp=po+100AND u1=1THEN mp=po:PRINT"Alric comes plodding up the steps.":RETurn
4850 IF (mp=po+11AND sw=1)OR(mp=po+9AND se1=1)THEN mp=po:PRINT"Alric walks up to you.":RETurn
4860 PRINT"There is no sign of Alric."
4865 END DEFine
4870 CLS
4880 PRINT" Do you wish to load a previously    stored character ?(Y or N)"
4890 h$=INKEY$(-1)
4900 IF h$="y"OR h$="Y"THEN load_:GO TO 190
4910 IF h$<>"n"AND h$<>"N"THEN GO TO 4890
4920 CLS
4930 PRINT"Enter your characters name<36 letters"
4940 INPUT na$
4950 IF LEN(na$)>35THEN GO TO 4920
4960 po=10:mo=0:od=0:id=0:oc=0:os=0:mt=0:ra=0:va=0:wi=0:tb=0:lp=RND(5 TO 30):gp=0:xe=0:le=1:ml=0:pm=-1:ni=0:CLS
4970 ma=0:mp=154:mah=RND(5TO 25):pp=0:th=0:max=lp:ch=0:bt=0:oxo=0:waes=0:og=0:nn=0:tt=0:sk=0
4980 GO TO 190
4990 DEFine PROCedure load_
5000 CLS
5010 PRINT" Insert cartridge in microdrive 1 andpress a key.":PAUSE
5020 CLS:PRINT"Loading...."
5030 OPEN_IN #5,flp1_char_data
5040 INPUT #5,na$,po,mo,od,id,oc,os,mt,ra,va,wi,tb,lp,gp,xe,le,ml,pm,ni,mah,pp,th,max,ch,bt,oxo,waes,og,nn,tt,sk,ma,mp
5050 FOR i=1TO 12
5060 INPUT #5,b(i)
5070 NEXT i
5080 CLOSE #5
5090 STAT
5100 END DEFine
5110 DEFine PROCedure instructIons
5120 PAPER 0:MODE 8
5130 title
5140 PRINT"  This game combines the realism of  the text adventure with the action ofthe arcade.In it you control the     computer as in a usual adventure,but from time to time you will come      across evil monsters whom you must   defeat,if you are to achieve your    goal."
5150 PRINT"  The screen is split into three     windows.The top window gives a       description of your surroundings,the middle your responses or computer    prompts and the lower,anything you   are carrying."
5160 PRINT"  In 'slow mode' you give the        computer short two or three word     sentances describing the course of   your action.Eg 'go north','climb tree'help'.All commands must be entered  in lower case,eg'abcd' and not 'ABCD'"
5170 PRINT:PRINT"  Press a key to continue.":PAUSE
5180 CLS:title:PRINT"  Some more common commands can be   abbreviated such as,'look' can be    simply entered as 'l' followed by    'enter'.The computers vocabulary is  limited but there is still a great   deal that can be achieved.Some of themore essential commands are 'load',  'save','restore','quit' and of course'get gold'!"
5190 PRINT"  In 'combat mode' you enter single  key commands to choose your course ofaction,and you must decide quickly   otherwise you'll be dead before you  have made up your mind!Enter the wordor number shown in brackets.Your     'life points' shown in the lower     window are an indication of your     strength.If and when these get to    zero you are dead! When a monster    attacks it will do so much damage,   this is subtracted from your life    points score....."
5200 PAUSE:CLS:title
5210 PRINT"  Likewise you can do damage on the  monsters.As you progress you gain    experience points for everything you do.Each time you gain 1000 you are   said to have 'gone up a level' and   automatically gain some extra life   points,as you have become more       skilled in survival and combat."
5220 PRINT"  If you gain some magic weapons or  armour you will do extra damage in   battle,or be protected from any evil creatures.Finally,may your sword staybloody and your luck remain true!    The story so far...."
5230 PRINT"  Having come of age,and being       generally very bored by your         lifestyle you leave your hometown to seek your fame and fortune.After two months of aimless wandering you come to a small town at the foot of a     rugged mountain.":PAUSE:CLS:title:PRINT"  You are intrigued as the local folktell you of the strange undeground   dungeon,its entrance half-way up the mountain.They say that there is greattreasure to be claimed but that it isguarded by hundreds of foul monsters,all controlled for some devious      purpose by a wizard who has been seen"
5240 PRINT"by no one for years.You learn that   located somewhere in the mountain is a tomb,in it lies a long dead prince and all his vast wealth.They tell youthat many have gone on the perilous  journey,but few return,and those thatdo are unrecognizable.The people     plead with you to turn back,but you  are determined to find adventure.Witha look of sorrow on their faces they give you two magical potions and wishyou good luck.You turn and begin the ascent....."
5250 PAUSE:CLS:PRINT"           PLEASE WAIT":END DEFine
5260 DEFine PROCedure title
5270 CSIZE 3,1:INK 2:PRINT "  THE LOST TOMB OF ORNAK":CSIZE 2,0:INK 7:END DEFine
5280 DEFine PROCedure save_
5290 CLS
5300 PRINT" Insert cartridge in microdrive 1 andpress a key.":PAUSE
5310 CLS:PRINT"Saving....."
5320 DELETE flp1_char_data:OPEN_NEW #5,flp1_char_data
5330 PRINT #5,na$:PRINT #5,po:PRINT #5,mo:PRINT #5,od:PRINT #5,id:PRINT #5,oc:PRINT #5,os:PRINT #5,mt:PRINT #5,ra:PRINT #5,va:PRINT #5,wi:PRINT #5,tb:PRINT #5,lp:PRINT #5,gp:PRINT #5,xe:PRINT #5,le:PRINT #5,ml:PRINT #5,pm:PRINT #5,ni:PRINT #5,mah:PRINT #5,pp:PRINT #5,th:PRINT #5,max:PRINT #5,ch:PRINT #5,bt:PRINT #5,oxo:PRINT #5,waes:PRINT #5,og:PRINT #5,nn:PRINT #5,tt:PRINT #5,sk:PRINT #5,ma:PRINT #5,mp
5340 FOR i=1TO 12
5350 PRINT #5,b(i)
5360 NEXT i
5370 CLOSE #5
5380 END DEFine
5390 DATA "ORC",7,"GOBLIN",5,"HOBGOBLIN",6,"GHOUL",11,"GIANT RAT",4,"GIANT BAT",8,"TROGLADYTE",13,"BALROG",19,"SKELETON",9,"ZOMBIE",4,"WIZARD",34,"OGRE",22,"VAMPIRE",28
5400 DATA "KEY",-1,"KEY",-1,"SWORD",0,"ARMOUR",0,"HEALING POTION",0,"INVISIBILITY POTION",0,"MAGIC SWORD",-1,"MAGIC ARMOUR",-1,"HAMMER",141,"STAKE",141,"SPEAR",172,"GEM",-1
