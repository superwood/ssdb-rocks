/* control */
void initHashRanges(struct hashranges *ranges) {
    ranges->range = NULL;
    ranges->len = 0;
}

void resetHashRanges(struct hashranges *ranges) {
    zfree(ranges->range);
    initHashRanges(ranges);
}

void copyHashRanges(struct hashranges *src, struct hashranges *dst) {
    dst->range = src->range;
    dst->len = src->len;
}

void appendHashRanges(struct hashranges *ranges, unsigned int low, unsigned int high) {
    ranges->range = (struct hashrange*) zrealloc( (void*) (ranges->range),sizeof(struct hashrange)*(ranges->len+1));
    ranges->range[ranges->len].low = low;
    ranges->range[ranges->len].high = high;
    ranges->len++;
}

void mergeHashRanges(struct hashranges *ranges, struct hashranges *dst) {
    int i, init = 0;
    unsigned int low = 0, high = 0;
    for (i = 0; i < ranges->len; i++) {
        if (!init) {
            low = ranges->range[i].low;
            high = ranges->range[i].high;
            init = 1;
        } else if (high == ranges->range[i].low) {
            high = ranges->range[i].high;
        } else {
            appendHashRanges(dst, low, high);
            low = ranges->range[i].low;
            high = ranges->range[i].high;
        }
    }
    if (init)
        appendHashRanges(dst, low, high);
}

void addHashRanges(struct hashranges *src, struct hashranges *delta, struct hashranges *dst) {
    int i = 0, j = 0;
    unsigned int low = 0, high = 0;
    struct hashranges ranges;

    initHashRanges(&ranges);
    while (i < src->len && j < delta->len) {
        if (delta->range[j].low > src->range[i].low) {
            low = src->range[i].low;
            high = src->range[i].high;
            i++;
        } else {
            low = delta->range[j].low;
            high = delta->range[j].high;
            j++;
        }
        appendHashRanges(&ranges, low, high);
    }
    while (i < src->len) {
        appendHashRanges(&ranges, src->range[i].low, src->range[i].high);
        i++;
    }
    while (j < delta->len) {
        appendHashRanges(&ranges, delta->range[j].low, delta->range[j].high);
        j++;
    }
    mergeHashRanges(&ranges, dst);
    resetHashRanges(&ranges);
}

void delHashRanges(struct hashranges *src, struct hashranges *delta, struct hashranges *dst) {
    int i = 0, j = 0, init = 0;
    unsigned int low = 0, high = 0;

    while (i < src->len && j < delta->len) {
        if (!init) {
            low = src->range[i].low;
            high = src->range[i].high;
            init = 1;
        }

        if (high && delta->range[j].low >= high) {
            appendHashRanges(dst, low, high);
            init = 0;
            i++;
            continue;
        } else if (delta->range[j].low == low) {
            low = delta->range[j].high;
            j++;
        } else if (delta->range[j].low > low) {
            appendHashRanges(dst, low, delta->range[j].low);
            low = delta->range[j].high;
            j++;
        }

        if(low == high) {
            i++;
            init = 0;
        }
    }
    if (init) {
        appendHashRanges(dst, low, high);
        i++;
    }
    while (i < src->len) {
        appendHashRanges(dst, src->range[i].low, src->range[i].high);
        i++;
    }
}

int includeHashRanges(struct hashranges *ranges, struct hashranges *delta) {
    int i = 0, j = 0;

    while (i < ranges->len && j < delta->len) {
        if (ranges->range[i].high && delta->range[j].low >= ranges->range[i].high) {
            i++;
            continue;
        } else if (delta->range[j].low >= ranges->range[i].low &&
                   (!ranges->range[i].high ||
                    (delta->range[j].high &&
                     delta->range[j].high <= ranges->range[i].high))) {
            j++;
            continue;
        } else {
            return 0;
        }
    }
    if(j == delta->len)
        return 1;
    return 0;
}

int excludeHashRanges(struct hashranges *ranges, struct hashranges *delta) {
    int i = 0, j = 0;

    while (i < ranges->len && j < delta->len) {
        if (ranges->range[i].high && delta->range[j].low >= ranges->range[i].high) {
            i++;
            continue;
        } else if (delta->range[j].high && delta->range[j].high <= ranges->range[i].low) {
            j++;
            continue;
        } else {
            return 0;
        }
    }
    return 1;
}

int inHashRanges(struct hashranges *ranges, unsigned int hash) {
    int i;

    for (i = 0; i < ranges->len; i++) {
        if (ranges->range[i].high && hash >= ranges->range[i].high)
            continue;
        else if (hash >= ranges->range[i].low)
            return 1;
        else
            return 0;
    }
    return 0;
}






static int proc_addhrange(Server *serv, Link *link, const Request &req, Response *resp){
	//addhrange long long long long : (nums of long) %2 ==0;
	if(req.size() < 3){
		resp->push_back("client_error [addhrange long  long]");
	}else{
		struct hashranges ranges;
		if(  REDIS_DIST_NONE != serv->ssdb->diststate ){//at reb status, so can not do reb again
			resp->push_back("err! can not set range,the db is rebalancing");
			return 0;
		}
		initHashRanges(&ranges);
		int nums = (req.size()-1);
		if( 0 != nums%2){//range
			resp->push_back("err! the nums of rang %2 != 0");
			return 0;
		}
		for(unsigned int i=1; i<req.size();){
			char *eptr;
			unsigned int low = strtoul( req[i].data(),&eptr, 10 );
			i++;
			unsigned int hight = strtoul(req[i].data(), &eptr,10);
			i++;
			appendHashRanges(&ranges, low, hight);
		}
		if( 0 != excludeHashRanges(&serv->ssdb->hash_range, &ranges)){
			resp->push_back("err! Hash range is invalid");
			return 0;
		}
		/*no lock!   some thing will wrong!  must change to double buff+lock
		  but in redis ,which has only one thread it works.
		 */
		resetHashRanges(&serv->ssdb->hash_range);
		resetHashRanges(&serv->ssdb->delta_range);
		resetHashRanges(&serv->ssdb->switch_range);
		copyHashRanges(&ranges, &serv->ssdb->hash_range);

		resp->push_back("ok");
	}

	return 0;
}


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
		resp->push_back("client_error [sethrange long long]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		//val2.append(req[2].data(),req[2].size());
		if( "none" == val1  ){
			resetHashRanges(&serv->ssdb->hash_range);
			resetHashRanges(&serv->ssdb->delta_range);
			resetHashRanges(&serv->ssdb->switch_range);
		}else{
			struct hashranges ranges;
			if(  REDIS_DIST_NONE != serv->ssdb->diststate ){//at reb status, so can not do reb again
				resp->push_back("err! can not set range,the db is rebalancing");
				return 0;
			}

			initHashRanges(&ranges);
			int nums = (req.size()-1);
			if( 0 != nums%2){//range
				resp->push_back("err! the nums of rang %2 != 0");
				return 0;
			}
			for(unsigned int i=1; i<req.size();){
				char *eptr;
				unsigned int low = strtoul( req[i].data(),&eptr, 10 );
				i++;
				unsigned int hight = strtoul(req[i].data(), &eptr,10);
				i++;
				appendHashRanges(&ranges, low, hight);
			}

			resetHashRanges(&serv->ssdb->hash_range);
			resetHashRanges(&serv->ssdb->delta_range);
			resetHashRanges(&serv->ssdb->switch_range);
			copyHashRanges(&ranges, &serv->ssdb->hash_range);
		}

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



static int proc_delhrange(Server *serv, Link *link, const Request &req, Response *resp){
	if(req.size() < 2){
		resp->push_back("client_error [delhrange long long]");
	}else{
		std::string val1;
		std::string val2;
		val1.append(req[1].data(), req[1].size());
		val2.append(req[2].data(),req[2].size());

		struct hashranges ranges;
		if(  REDIS_DIST_NONE != serv->ssdb->diststate ){//at reb status, so can not do reb again
			resp->push_back("err! can not del range,the db is rebalancing");
			return 0;
		}

		initHashRanges(&ranges);
		int nums = (req.size()-1);
		if( 0 != nums%2){//range
			resp->push_back("err! the nums of rang %2 != 0");
			return 0;
		}
		for(unsigned int i=1; i<req.size();){
			char *eptr;
			unsigned int low = strtoul( req[i].data(),&eptr, 10 );
			i++;
			unsigned int hight = strtoul(req[i].data(), &eptr,10);
			i++;
			appendHashRanges(&ranges, low, hight);
		}
		if( 0 != includeHashRanges(&serv->ssdb->hash_range, &ranges) ){
			resp->push_back("delete rang fail, Hash range is invalid");
			return 0;
		}

		//resetHashRanges(&serv->ssdb->dist_hash_range);
		resetHashRanges(&serv->ssdb->delta_range);
		copyHashRanges(&serv->ssdb->delta_range, &ranges);
		resetHashRanges(&serv->ssdb->switch_range);
		delHashRanges(&serv->ssdb->hash_range,&serv->ssdb->delta_range,&serv->ssdb->switch_range);
		//serv->ssdb->dist_mode = REDIS_DIST_MODE_SRC;

		resp->push_back("ok");
	}
	return 0;
}








