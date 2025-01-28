package org.thewrooms.openbench.service;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.thewrooms.openbench.repository.UserRepository;

@Service
public class UserService {

    @Autowired
    private UserRepository userRepository;

}