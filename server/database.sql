drop database if exists mydb;
create database mydb;

use mydb;

drop table if exists info;
create table info (
id char(20) not null,
passwd char(20) not null,
nwin SMALLINT not null,
nfail SMALLINT not null
);

insert into info values('didi','rkmfvelpiiqjipgk',5,5);
insert into info values('gege','rkmfvelpiiqjipgk',4,4);
insert into info values('jiejie','rkmfvelpiiqjipgk',3,3);
