
CREATE TABLE `jobs` (
	`id` int(10) unsigned NOT NULL auto_increment,
	`job_type` varchar(10) NOT null,
	`job` varchar(255) NOT null,
	`insert_date` datetime NOT null,
	`start_date` datetime,
	`finish_date` datetime,
	`state` varchar(10),
	`machine` varchar(20),
	`batch_date` date NOT null,
	PRIMARY KEY (`id`)
) ENGINE=InnoDB;
	
