<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.6.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="SuperFeed">
<packages>
<package name="PUSH">
<text x="-2.54" y="6.35" size="1.27" layer="25">&gt;NAME</text>
<text x="-2.54" y="3.81" size="1.27" layer="27">&gt;VALUE</text>
<pad name="1" x="-3.2512" y="2.2606" drill="0.8" shape="long"/>
<pad name="2" x="3.2512" y="2.2606" drill="0.8" shape="long"/>
<pad name="3" x="-3.2512" y="-2.2606" drill="0.8" shape="long"/>
<pad name="4" x="3.2512" y="-2.2606" drill="0.8" shape="long"/>
<wire x1="-2.54" y1="-2.54" x2="-2.54" y2="2.54" width="0.4064" layer="21"/>
<wire x1="-2.54" y1="2.54" x2="2.54" y2="2.54" width="0.4064" layer="21"/>
<wire x1="2.54" y1="2.54" x2="2.54" y2="-2.54" width="0.4064" layer="21"/>
<wire x1="2.54" y1="-2.54" x2="-2.54" y2="-2.54" width="0.4064" layer="21"/>
</package>
<package name="MA04-1W">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<wire x1="5.08" y1="-1.016" x2="5.08" y2="1.778" width="0.1524" layer="21"/>
<wire x1="5.08" y1="-1.016" x2="2.54" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="2.54" y1="-1.016" x2="2.54" y2="1.778" width="0.1524" layer="21"/>
<wire x1="2.54" y1="1.778" x2="5.08" y2="1.778" width="0.1524" layer="21"/>
<wire x1="2.54" y1="-1.016" x2="0" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="0" y1="1.778" x2="2.54" y2="1.778" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="-1.016" x2="-2.54" y2="1.778" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="-1.016" x2="-5.08" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="-1.016" x2="-5.08" y2="1.778" width="0.1524" layer="21"/>
<wire x1="-5.08" y1="1.778" x2="-2.54" y2="1.778" width="0.1524" layer="21"/>
<wire x1="0" y1="-1.016" x2="0" y2="1.778" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="-1.016" x2="0" y2="-1.016" width="0.1524" layer="21"/>
<wire x1="-2.54" y1="1.778" x2="0" y2="1.778" width="0.1524" layer="21"/>
<wire x1="-3.81" y1="2.54" x2="-3.81" y2="7.239" width="0.6604" layer="21"/>
<wire x1="-1.27" y1="2.54" x2="-1.27" y2="7.239" width="0.6604" layer="21"/>
<wire x1="1.27" y1="2.54" x2="1.27" y2="7.239" width="0.6604" layer="21"/>
<wire x1="3.81" y1="2.54" x2="3.81" y2="7.239" width="0.6604" layer="21"/>
<pad name="4" x="3.81" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="3" x="1.27" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="2" x="-1.27" y="0" drill="1.016" shape="long" rot="R90"/>
<pad name="1" x="-3.81" y="0" drill="1.016" shape="long" rot="R90"/>
<text x="-5.08" y="-2.54" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-3.175" y="-0.508" size="1.27" layer="21" ratio="10" rot="R90">1</text>
<text x="-0.635" y="-0.508" size="1.27" layer="21" ratio="10" rot="R90">2</text>
<text x="1.905" y="-0.508" size="1.27" layer="21" ratio="10" rot="R90">3</text>
<text x="4.445" y="-0.508" size="1.27" layer="21" ratio="10" rot="R90">4</text>
<text x="-5.08" y="-3.81" size="1.27" layer="27" ratio="10">&gt;VALUE</text>
<rectangle x1="-4.1402" y1="1.778" x2="-3.4798" y2="2.54" layer="21"/>
<rectangle x1="-1.6002" y1="1.778" x2="-0.9398" y2="2.54" layer="21"/>
<rectangle x1="0.9398" y1="1.778" x2="1.6002" y2="2.54" layer="21"/>
<rectangle x1="3.4798" y1="1.778" x2="4.1402" y2="2.54" layer="21"/>
<rectangle x1="3.4798" y1="-0.4064" x2="4.1402" y2="0.254" layer="21"/>
<rectangle x1="0.9398" y1="-0.4064" x2="1.6002" y2="0.254" layer="21"/>
<rectangle x1="3.4798" y1="-0.3302" x2="4.1402" y2="0.8636" layer="51"/>
<rectangle x1="0.9398" y1="-0.3302" x2="1.6002" y2="0.8636" layer="51"/>
<rectangle x1="-1.6002" y1="-0.4064" x2="-0.9398" y2="0.254" layer="21"/>
<rectangle x1="-4.1402" y1="-0.4064" x2="-3.4798" y2="0.254" layer="21"/>
<rectangle x1="-1.6002" y1="-0.3302" x2="-0.9398" y2="0.8636" layer="51"/>
<rectangle x1="-4.1402" y1="-0.3302" x2="-3.4798" y2="0.8636" layer="51"/>
</package>
</packages>
<symbols>
<symbol name="PUSH">
<pin name="1" x="-12.7" y="2.54" visible="off" length="middle"/>
<pin name="2" x="2.54" y="2.54" visible="off" length="middle" rot="R180"/>
<wire x1="-7.62" y1="2.54" x2="-2.54" y2="5.08" width="0.254" layer="94"/>
<text x="-10.16" y="10.16" size="1.27" layer="95">&gt;NAME</text>
<text x="-10.16" y="7.62" size="1.27" layer="96">&gt;VALUE</text>
</symbol>
<symbol name="MA04-1">
<wire x1="3.81" y1="-7.62" x2="-1.27" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="1.27" y1="0" x2="2.54" y2="0" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-2.54" x2="2.54" y2="-2.54" width="0.6096" layer="94"/>
<wire x1="1.27" y1="-5.08" x2="2.54" y2="-5.08" width="0.6096" layer="94"/>
<wire x1="-1.27" y1="5.08" x2="-1.27" y2="-7.62" width="0.4064" layer="94"/>
<wire x1="3.81" y1="-7.62" x2="3.81" y2="5.08" width="0.4064" layer="94"/>
<wire x1="-1.27" y1="5.08" x2="3.81" y2="5.08" width="0.4064" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.6096" layer="94"/>
<text x="-1.27" y="-10.16" size="1.778" layer="96">&gt;VALUE</text>
<text x="-1.27" y="5.842" size="1.778" layer="95">&gt;NAME</text>
<pin name="1" x="7.62" y="-5.08" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="2" x="7.62" y="-2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="3" x="7.62" y="0" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
<pin name="4" x="7.62" y="2.54" visible="pad" length="middle" direction="pas" swaplevel="1" rot="R180"/>
</symbol>
<symbol name="GND">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="-2.54" y="-2.54" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="PUSH">
<gates>
<gate name="G$1" symbol="PUSH" x="5.08" y="-2.54"/>
</gates>
<devices>
<device name="" package="PUSH">
<connects>
<connect gate="G$1" pin="1" pad="1 2"/>
<connect gate="G$1" pin="2" pad="3 4"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="MA04-1W" prefix="SV" uservalue="yes">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="MA04-1" x="0" y="0"/>
</gates>
<devices>
<device name="" package="MA04-1W">
<connects>
<connect gate="1" pin="1" pad="1"/>
<connect gate="1" pin="2" pad="2"/>
<connect gate="1" pin="3" pad="3"/>
<connect gate="1" pin="4" pad="4"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GND" prefix="GND">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="GND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="U$1" library="SuperFeed" deviceset="PUSH" device=""/>
<part name="U$2" library="SuperFeed" deviceset="PUSH" device=""/>
<part name="U$3" library="SuperFeed" deviceset="PUSH" device=""/>
<part name="SV1" library="SuperFeed" deviceset="MA04-1W" device=""/>
<part name="GND1" library="SuperFeed" deviceset="GND" device=""/>
<part name="GND2" library="SuperFeed" deviceset="GND" device=""/>
</parts>
<sheets>
<sheet>
<plain>
</plain>
<instances>
<instance part="U$1" gate="G$1" x="2.54" y="53.34"/>
<instance part="U$2" gate="G$1" x="2.54" y="40.64"/>
<instance part="U$3" gate="G$1" x="2.54" y="25.4"/>
<instance part="SV1" gate="1" x="-33.02" y="43.18"/>
<instance part="GND1" gate="1" x="12.7" y="22.86"/>
<instance part="GND2" gate="1" x="-22.86" y="27.94"/>
</instances>
<busses>
</busses>
<nets>
<net name="N$1" class="0">
<segment>
<pinref part="U$3" gate="G$1" pin="1"/>
<wire x1="-10.16" y1="27.94" x2="-15.24" y2="27.94" width="0.1524" layer="91"/>
<wire x1="-15.24" y1="27.94" x2="-15.24" y2="40.64" width="0.1524" layer="91"/>
<pinref part="SV1" gate="1" pin="2"/>
<wire x1="-25.4" y1="40.64" x2="-15.24" y2="40.64" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$3" class="0">
<segment>
<pinref part="U$1" gate="G$1" pin="1"/>
<wire x1="-10.16" y1="55.88" x2="-10.16" y2="45.72" width="0.1524" layer="91"/>
<pinref part="SV1" gate="1" pin="4"/>
<wire x1="-10.16" y1="45.72" x2="-25.4" y2="45.72" width="0.1524" layer="91"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="GND1" gate="1" pin="GND"/>
<wire x1="12.7" y1="25.4" x2="12.7" y2="27.94" width="0.1524" layer="91"/>
<pinref part="U$1" gate="G$1" pin="2"/>
<wire x1="12.7" y1="27.94" x2="12.7" y2="43.18" width="0.1524" layer="91"/>
<wire x1="12.7" y1="43.18" x2="12.7" y2="55.88" width="0.1524" layer="91"/>
<wire x1="12.7" y1="55.88" x2="5.08" y2="55.88" width="0.1524" layer="91"/>
<pinref part="U$2" gate="G$1" pin="2"/>
<wire x1="5.08" y1="43.18" x2="12.7" y2="43.18" width="0.1524" layer="91"/>
<pinref part="U$3" gate="G$1" pin="2"/>
<wire x1="5.08" y1="27.94" x2="12.7" y2="27.94" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="SV1" gate="1" pin="1"/>
<pinref part="GND2" gate="1" pin="GND"/>
<wire x1="-25.4" y1="38.1" x2="-22.86" y2="38.1" width="0.1524" layer="91"/>
<wire x1="-22.86" y1="38.1" x2="-22.86" y2="30.48" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<pinref part="U$2" gate="G$1" pin="1"/>
<pinref part="SV1" gate="1" pin="3"/>
<wire x1="-10.16" y1="43.18" x2="-25.4" y2="43.18" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
