% logThingSpeakData
% This function is used by PeopleCounterDemo.m to write data to a
% ThingSpeak channel. 
%
% Copyright 2016 The MathWorks, Inc

function logThingSpeakData(tracker)

channelID = 1340677;
writeKey = 'DA2AXX06ZYZMGJTM';

if ischar(tracker) && strcmp(tracker,'reset')
    thingSpeakWrite(channelID,[0 0],'WriteKey',writeKey)
else
    data = [0 0];
    data(1) = tracker.NextId-1;
    data(2) = numel(tracker.BoxIds);
    
    try
    thingSpeakWrite(channelID,data,'WriteKey',writeKey,'Timeout',5)
    catch
    end
end