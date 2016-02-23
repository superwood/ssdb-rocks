/* control */

static int proc_SLAVEOF(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 3){
		resp->push_back("client_error [SLAVEOF ip port] [SLAVEOF NO ONE]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		val2.append(req[2].data(),req[2].size());
		if( "NO" == val1  && "ONE" == val2 ){
			int ret = serv->ssdb->slave_stop();
			if( 0 == ret ){
				resp->push_back("ok");
			}else{
				resp->push_back("no slave of");
			}
			return ret;
		}else{
			int ret = serv->ssdb->slave_of(val1, val2);
			if( 0 == ret){
				resp->push_back("ok");
			}else{
				resp->push_back("slave of fail");
			}
			return ret;
		}
	}
	return 0;
}

static int proc_setsyncfact(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 2){
		resp->push_back("client_error [setsyncfact factor]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		/*some thing todo */	
		resp->push_back("ok");
	}
	return 0;
}

static int proc_sethrange(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 2){
		resp->push_back("client_error [setsyncfact factor]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		/*some thing todo */	
		resp->push_back("ok");
	}
	return 0;
}





static int proc_slaveof(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 3){
		resp->push_back("client_error [SLAVEOF ip port] [SLAVEOF NO ONE]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		val2.append(req[2].data(),req[2].size());
		if( "NO" == val1  && "ONE" == val2 ){
			int ret = serv->ssdb->slave_stop();
			if( 0 == ret ){
				resp->push_back("ok");
			}else{
				resp->push_back("no slave of");
			}
			return ret;
		}else{
			int ret = serv->ssdb->slave_of(val1, val2);
			if( 0 == ret){
				resp->push_back("ok");
			}else{
				resp->push_back("slave of fail");
			}
			return ret;
		}
	}
	return 0;
}

static int proc_rebalancefrom(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 2){
		resp->push_back("client_error [rebalancefrom ip port] [rebalancefrom NO ONE]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		val2.append(req[2].data(),req[2].size());
		if( "NO" == val1  && "ONE" == val2 ){
			int ret = 0;//serv->ssdb->slave_stop();
			if( 0 == ret ){
				resp->push_back("ok");
			}else{
				resp->push_back("no rebalancefrom");
			}
			return ret;
		}else{
			int ret = 0;//= serv->ssdb->slave_of(val1, val2);
			if( 0 == ret){
				resp->push_back("ok");
			}else{
				resp->push_back("rebalancefrom  fail");
			}
			return ret;
		}
	}
	return 0;
}

static int proc_rebalanceto(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 2){
		resp->push_back("client_error [rebalanceto ip port] [rebalanceto NO ONE]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		val2.append(req[2].data(),req[2].size());
		if( "NO" == val1  && "ONE" == val2 ){
			int ret = 0;//serv->ssdb->slave_stop();
			if( 0 == ret ){
				resp->push_back("ok");
			}else{
				resp->push_back("no rebalanceto");
			}
			return ret;
		}else{
			int ret =0;//= serv->ssdb->slave_of(val1, val2);
			if( 0 == ret){
				resp->push_back("ok");
			}else{
				resp->push_back("rebalanceto  fail");
			}
			return ret;
		}
	}
	return 0;
}



static int proc_addhrange(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 2){
		resp->push_back("client_error [addhrange long long]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		val2.append(req[2].data(),req[2].size());
		resp->push_back("ok");
	}
	return 0;
}

static int proc_delhrange(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 2){
		resp->push_back("client_error [delhrange long long]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		val2.append(req[2].data(),req[2].size());
		resp->push_back("ok");
	}
	return 0;
}

